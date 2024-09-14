#include "h/context.h"
#include "h/eval.h"
#include "h/builtins.h"
#include "h/parse.h"
#include "h/reader.h"

void collaps(context* ctx) {
	while (1) {
		tFunc* f = NULL;
		unit p[MAX_ARGC] = {0};
		size_t i = 0;
		for (; i < stacksize(ctx) && (f ? i < f->argc + 1 : i < MAX_ARGC + 1); i++) {
			if (is(*stackidx(ctx, i), TYPE_Func)) {
				if (f) return;
				f = get(tFunc*, *stackidx(ctx, i));
			} else {
				p[MAX_ARGC - i - 1 + !!f] = *stackidx(ctx, i);
			}
		}
		if (!f || i != f->argc + 1)
			return;

		drop(&ctx->stack, i);
		unit res = f->__invoke(&p[MAX_ARGC - i + 1]);
		if (!is(res, TYPE_Undefined))
			push(&ctx->stack, res);
	}
}

void eval(context* ctx) {
	while (!ctx->input.eof) {
		unit res = make(TYPE_Undefined);
		switch (curr(ctx)) {
			case '\0' ... ' ': // ERROR: unrecognized character
				next(ctx);
				continue;
			case '#':
				parse_comment(ctx);
				continue;
			case ',':
				next(ctx);
				ctx->base = ctx->stack.count;
				continue;

			case ')':
			case ']':
				next(ctx);
				// if (ctx->closer != curr(ctx)) ERROR: mismatched brackets
				return;

			case '(':
			case '[':
				parse_bracket(ctx);
				break;
			case '0' ... '9':
			case '-':
			case '.':
				res = parse_num(ctx);
				break;
			case '"':
				res = parse_string(ctx);
				break;
			case 'a' ... 'z':
			case 'A' ... 'Z':
			case '_':
				res = parse_func(ctx);
				break;
			default:
				res = parse_op(ctx);
				break;
		}

		if (!is(res, TYPE_Undefined))
			push(&ctx->stack, res);
		collaps(ctx);
	}
}
