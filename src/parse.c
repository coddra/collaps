#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "h/eval.h"
#include "h/parse.h"
#include "h/builtins.h"
#include "h/reader.h"
#include "h/util.h"

void parse_comment(context* ctx) {
    enter_token(ctx);
    while (!ctx->input.eof && next(ctx) != '\n');
}

void parse_bracket(context* ctx) {
    char c = curr(ctx);
    context child = {
        .parent = ctx,
        .environment = ctx->environment,
        .input = ctx->input,
        .location = ctx->location,
        .token_location = ctx->token_location,
        .stack = c == '[' ? list_new() : ctx->stack,
        .base = c == '[' ? 0 : ctx->stack.count,
        .closer = c == '[' ? ']' : ')',
    };
    next(&child);
    eval(&child);
    
    if (c == '[')
        push(&ctx->stack, mklistalloc(child.stack));
    else
        ctx->stack = child.stack;
    ctx->input = child.input;
    ctx->location = child.location;
    ctx->token_location = child.token_location;
}

unit parse_num(context* ctx) {
    enter_token(ctx);

    uint64_t base = 10;
    bool negative = false;
    bool is_float = false;

    if (curr(ctx) == '-') {
        next(ctx);
        if ((curr(ctx) < '0' || curr(ctx) > '9') && curr(ctx) != '.')
            return make(TYPE_Func, (uc){ .p = &ops[OP_SUB] });
        negative = true;
    }

    if (curr(ctx) == '0') {
        switch (next(ctx)) {
            case 'x':
                base = 24;
            case 'b':
                base -= 6;
            case 'o':
                base -= 2;
                break;
        }
        next(ctx);
    }

    while (is_digit(curr(ctx), base))
        next(ctx);
    
    if (curr(ctx) == '.' || 
        (base == 10 && (curr(ctx) == 'e' || curr(ctx) == 'E')) ||
        (base == 16 && (curr(ctx) == 'p' || curr(ctx) == 'P')))
        is_float = true;

    if (is_float) {
        char* end = NULL;
        double val = strtod(token_start(ctx), &end);
        ctx->input.pos = ctx->input.tok + (end - token_start(ctx));
        return make(TYPE_Float, (uc){ .d = val });
    }

    if (negative) ctx->input.tok++;
    if (base != 10) ctx->input.tok += 2;
    char* end = NULL;
    int64_t val = strtoll(token_start(ctx), &end, base);
    ctx->input.pos = ctx->input.tok + (end - token_start(ctx));
    if (negative) val = -val;
    return make(TYPE_Int, (uc){ .i = val });
}

unit parse_string(context* ctx) {
    next(ctx);
    enter_token(ctx);

	char* res = NULL;
	size_t length = 0;
	while (1) {
        if (!ctx->input.eof && curr(ctx) != '"' && curr(ctx) != '\\') {
            next(ctx);
            continue;
        }
		
		res = (char*)realloc(res, length + token_length(ctx) + 1);
		memcpy(res + length, token_start(ctx), token_length(ctx));
		length += token_length(ctx);
		res[length] = '\0';
        //if (ctx->input.eof) ERROR: eof in string
        
        if (curr(ctx) != '\\') {
            next(ctx);
            return make(TYPE_String, (uc){ .s = res });
        }

        char unicodelenght = 8;
        int codepoint = 0;
		switch (next(ctx)) {
            case '\0':
                // ERROR: eof in escape
                break;
#define ESCAPE_CASE(c, r) case c: res[length] = r; break
            ESCAPE_CASE('\\', '\\');
            ESCAPE_CASE('"', '"');
            ESCAPE_CASE('n', '\n');
            ESCAPE_CASE('r', '\r');
            ESCAPE_CASE('t', '\t');
            ESCAPE_CASE('b', '\b');
            ESCAPE_CASE('f', '\f');
            ESCAPE_CASE('v', '\v');
            ESCAPE_CASE('a', '\a');
            ESCAPE_CASE('e', '\e');
#undef ESCAPE_CASE
			case '0' ... '9':
				res[length] = curr(ctx) - '0';
				break;
			case 'x':
				res[length] = (char)((fromhex(next(ctx)) << 4) + fromhex(next(ctx))); // TODO: ERROR: eof in hex escape
				break;
            case 'u':
                unicodelenght = 4;
            case 'U':
                for (size_t i = 0; i < unicodelenght; i++) {
                    if (next(ctx) == '\0')
                        break; // ERROR: eof in unicode escape
                    codepoint = (codepoint << 4) + fromhex(curr(ctx));
                }
                res = realloc(res, length + unicodelenght / 2);
                char shift = unicodelenght == 4 ? 6 : 18;
                char lead = unicodelenght == 4 ? 0xC0 : 0xF0;
                res[length] = lead | (codepoint >> shift);
                for (size_t i = 1; i < unicodelenght / 2; i++)
                    res[length + i] = 0x80 | ((codepoint >> (shift -= 6)) & 0x3F);
                length += unicodelenght / 2 - 1;
                break;
			default:
                // ERROR: unknown escape
				res[length] = curr(ctx);
				break;
		}
        next(ctx);
        enter_token(ctx);
		length++;
	}
}

unit parse_op(context* ctx) {
    enter_token(ctx);

    while (token_length(ctx) <= OP_MAX_LENGTH && is_opchar(next(ctx)));

    unit op = vUndefined;
    for (; token_length(ctx) > 0 && op == vUndefined; back(ctx))
        op = resolve_symbol(ctx, token_start(ctx), token_length(ctx));
    next(ctx);

    return op;
}

unit parse_symbol(context* ctx) {
    enter_token(ctx);
    while ((curr(ctx) >= 'a' && curr(ctx) <= 'z') || 
           (curr(ctx) >= 'A' && curr(ctx) <= 'Z') || 
           (curr(ctx) >= '0' && curr(ctx) <= '9') || 
           curr(ctx) == '_')
        next(ctx);
    
    unit v = resolve_symbol(ctx, token_start(ctx), token_length(ctx));
    if (v != vUndefined)
        return v;
    char* symbol = (char*)malloc(token_length(ctx) + 1);
    symbol[token_length(ctx)] = '\0';
    memmove(symbol, token_start(ctx), token_length(ctx));
    return make(TYPE_Symbol, (uc){ .s = symbol });
}

unit parse_block(context* ctx) {
    enter_token(ctx);

    int level = 1;
    while (!ctx->input.eof && level > 0) {
        char c = next(ctx);
        level += c == '{' ? 1 : c == '}' ? -1 : 0;
    }

    // if (level != 0) ERROR: eof before }

    char* code = (char*)malloc(token_length(ctx) + 1);
    code[token_length(ctx)] = '\0';
    memmove(code, token_start(ctx), token_length(ctx));
    tBlock block = {
        .__type = make(TYPE_Type, (uc){ .p = &types[TYPE_Block] }),
        .code = make(TYPE_String, (uc){ .s = code }),
        .location = make_alloc(TYPE_Location, &ctx->token_location)
    };
    next(ctx);

    return make_alloc(TYPE_Block, &block);
}
