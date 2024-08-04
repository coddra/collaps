#include "h/context.h"
#include "h/eval.h"
#include "h/builtins.h"
#include "h/parse.h"
#include "h/reader.h"
#include "h/unit.h"

void collaps(context* ctx) {
	while (1) {
		func* f = NULL;
		unit p[MAX_ARGC] = {0};
		size_t i = 0;
		for (; i < stacksize(ctx) && (f ? i < f->argc + 1 : i < MAX_ARGC + 1); i++) {
			if (is(*stackidx(ctx, i), T_FUNC)) {
				if (f) return;
				f = getfunc(*stackidx(ctx, i));
			} else {
				p[MAX_ARGC - i - 1 + !!f] = *stackidx(ctx, i);
			}
		}
		if (!f || i != f->argc + 1)
			return;

		drop(&ctx->stack, i);
		unit res = f->invoke(&p[MAX_ARGC - i + 1]);
		if (!is(res, T_VOID))
			push(&ctx->stack, res);
	}
}

void eval(context* ctx) {
	while (!ctx->input.eof) {
		unit res = mkvoid();
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

		if (!is(res, T_VOID))
			push(&ctx->stack, res);
		collaps(ctx);
	}
}
