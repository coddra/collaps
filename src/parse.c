#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "h/parse.h"
#include "h/eval.h"
#include "h/op.h"
#include "h/reader.h"
#include "h/unit.h"
#include "h/util.h"

void parse_comment(context* ctx) {
    tokenstart(ctx);
    while (!ctx->eof && step(ctx) != '\n');
}

void parse_num(context* ctx) {
    tokenstart(ctx);

    uint64_t base = 10;
    bool negative = false;
    bool is_float = false;

    if (curr(ctx) == '-') {
        next(ctx);
        if ((curr(ctx) < '0' || curr(ctx) > '9') && curr(ctx) != '.') {
            push(mkfunc(&ops[OP_SUB]));
            return;
        }
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

    if (curr(ctx) == '.') {
        next(ctx);
        if (tokenlen(ctx) == 1 && (curr(ctx) < '0' || curr(ctx) > '9')) {
            push(mkfunc(&ops[OP_PRINT]));
            return;
        }
        is_float = true;
    }

    if ((base == 10 && (curr(ctx) == 'e' || curr(ctx) == 'E'))
            || (base == 16 && (curr(ctx) == 'p' || curr(ctx) == 'P')))
        is_float = true;

    if (is_float) {
        double val = strtod(ctx->tok, &ctx->pos);
        push(mkfloat(val));
    } else {
        if (negative) ctx->tok++;
        if (base != 10) ctx->tok += 2;
        int64_t val = strtoll(ctx->tok, &ctx->pos, base);
        if (negative) val = -val;
        push(mkint(val));
    }
}

void parse_string(context* ctx) {
    next(ctx);
    tokenstart(ctx);

	char *res = NULL;
	size_t length = 0;
	while (1) {
		if (ctx->eof || curr(ctx) == '"' || curr(ctx) == '\\') {
			res = (char*)realloc(res, length + tokenlen(ctx) + 1);
			memcpy(res + length, ctx->tok, tokenlen(ctx));
			length += tokenlen(ctx);
			res[length] = '\0';

            //if (ctx->eof) ERROR: eof in string
            
            if (curr(ctx) != '\\') {
                next(ctx);
                push(mkstr(res));
                return;
            }
		}

        if (step(ctx) != '\\') 
            continue;

        char unicodelenght = 8;
        int codepoint = 0;
		switch (curr(ctx)) {
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

void parse_op(context* ctx) {
    tokenstart(ctx);

    while (tokenlen(ctx) <= OP_MAX_LENGTH && is_opchar(next(ctx)));

    int op = -1;
    for (; tokenlen(ctx) > 0 && op < 0; back(ctx))
        op = binsearchfunc(ops, OP_COUNT, ctx->tok, tokenlen(ctx));
    next(ctx);

    if (op < 0) {
        // ERROR: unknown operator
        return;
    }
    
    push(mkfunc(&ops[op]));
}

void parse_func(context* ctx) {
    // ERROR: function parsing not implemented
}
