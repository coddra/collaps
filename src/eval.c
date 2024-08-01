#include "h/context.h"
#include "h/eval.h"
#include "h/builtins.h"
#include "h/parse.h"
#include "h/reader.h"
#include "h/unit.h"

void collapse(context* ctx) {
	while (1) {
		size_t i = 0;
		while (i < MAX_ARGC && i < stacksize(ctx) && !is(*stackidx(ctx, i), T_FUNC))
			i++;
		if (i >= stacksize(ctx) || !is(*stackidx(ctx, i), T_FUNC))
			return;

		func* func = getfunc(*stackidx(ctx, i));
		unit pars[MAX_ARGC];
		int p = func->argc - 1;
		bool op = 0;
		for (i = 0; i < func->argc + 1 && i < stacksize(ctx) && (!op || !is(*stackidx(ctx, i), T_FUNC)); i++) {
			if (is(*stackidx(ctx, i), T_FUNC))
				op = true;
			else
				pars[p--] = *stackidx(ctx, i);
		}
		if (p != -1)
			return;
		drop(&ctx->stack, func->argc + 1);
		unit res = func->invoke(pars);
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
		collapse(ctx);
	}
}
