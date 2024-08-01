#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "h/parse.h"
#include "h/builtins.h"
#include "h/reader.h"
#include "h/types.h"
#include "h/unit.h"
#include "h/util.h"

void parse_comment(context* ctx) {
    tokenstart(ctx);
    while (!ctx->input.eof && next(ctx) != '\n');
}

void parse_bracket(context* ctx) {
    char c = curr(ctx);
    next(ctx);
	
    size_t base = ctx->base;
	list stack = ctx->stack;
	char closer = ctx->closer;
    
    ctx->base = c == '(' ? ctx->stack.count : 0;
    if (c == '[')
        ctx->stack = list_new();
    ctx->closer = c == '(' ? ')' : ']';
    
    eval(ctx);
    
    ctx->base = base;
    ctx->closer = closer;
    if (c == '[') {
        list* l = malloc(sizeof(list));
        *l = ctx->stack;
        ctx->stack = stack;
        push(&ctx->stack, mklist(l));
    }
}

unit parse_num(context* ctx) {
    tokenstart(ctx);

    uint64_t base = 10;
    bool negative = false;
    bool is_float = false;

    if (curr(ctx) == '-') {
        next(ctx);
        if ((curr(ctx) < '0' || curr(ctx) > '9') && curr(ctx) != '.')
            return mkfunc(&ops[OP_SUB]);
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
        double val = strtod(token(ctx), &end);
        ctx->input.pos = ctx->input.tok + (end - token(ctx));
        return mkfloat(val);
    }

    if (negative) ctx->input.tok++;
    if (base != 10) ctx->input.tok += 2;
    char* end = NULL;
    int64_t val = strtoll(token(ctx), &end, base);
    ctx->input.pos = ctx->input.tok + (end - token(ctx));
    if (negative) val = -val;
    return mkint(val);
}

unit parse_string(context* ctx) {
    next(ctx);
    tokenstart(ctx);

	char *res = NULL;
	size_t length = 0;
	while (1) {
        if (!ctx->input.eof && curr(ctx) != '"' && curr(ctx) != '\\') {
            next(ctx);
            continue;
        }
		
		res = (char*)realloc(res, length + tokenlen(ctx) + 1);
		memcpy(res + length, token(ctx), tokenlen(ctx));
		length += tokenlen(ctx);
		res[length] = '\0';
        //if (ctx->input.eof) ERROR: eof in string
        
        if (curr(ctx) != '\\') {
            next(ctx);
            return mkstr(res);
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
        tokenstart(ctx);
		length++;
	}
}

unit parse_op(context* ctx) {
    tokenstart(ctx);

    while (tokenlen(ctx) <= OP_MAX_LENGTH && is_opchar(next(ctx)));

    int op = -1;
    for (; tokenlen(ctx) > 0 && op < 0; back(ctx))
        op = binsearchfunc(ops, OP_COUNT, token(ctx), tokenlen(ctx));
    next(ctx);

    if (op < 0) {
        // ERROR: unknown operator
        return mkvoid();
    }
    
    return mkfunc(&ops[op]);
}

unit parse_func(context* ctx) {
    tokenstart(ctx);
    while ((curr(ctx) >= 'a' && curr(ctx) <= 'z') || 
           (curr(ctx) >= 'A' && curr(ctx) <= 'Z') || 
           (curr(ctx) >= '0' && curr(ctx) <= '9') || 
           curr(ctx) == '_')
        next(ctx);
    
    int func = binsearchfunc(funcs, FUNC_COUNT, token(ctx), tokenlen(ctx));
    if (func < 0) {
        // ERROR: unknown function
        return mkvoid();
    }

    return mkfunc(&funcs[func]);
}
