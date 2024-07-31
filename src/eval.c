#include "h/context.h"
#include "h/eval.h"
#include "h/op.h"
#include "h/parse.h"
#include "h/reader.h"
#include "h/unit.h"

func ops[OP_COUNT] = {
#define OP(key, name, argc, ...) [OP_(name)] = { key, argc, true, &CAT(FUNC_, OP_(name)) },
#include "h/builtins.h"
#undef OP
};


void push(context* ctx, unit u) {
	if (ctx->stack.top - ctx->stack.origin >= STACK_SIZE) {
		fprintf(stderr, "Fatal: Stack overflow\n");
		return;
	}

	*(ctx->stack.top++) = u;
}

void collapse(context* ctx) {
	while (1) {
		size_t i = 0;
		while (i < MAX_ARGC && i < stacksize(ctx) && !is(ctx->stack.top[-i - 1], T_FUNC))
			i++;
		if (i >= stacksize(ctx) || !is(ctx->stack.top[-i - 1], T_FUNC))
			return;

		func* func = getfunc(ctx->stack.top[-i - 1]);
		unit pars[MAX_ARGC];
		int p = func->argc - 1;
		bool op = 0;
		for (int i = 1; i <= func->argc + 1 && i <= stacksize(ctx) && (!op || !is(*(ctx->stack.top - i), T_FUNC)); i++) {
			if (is(*(ctx->stack.top - i), T_FUNC))
				op = true;
			else
				pars[p--] = *(ctx->stack.top - i);
		}
		if (p != -1)
			break;
		ctx->stack.top -= func->argc + 1;
		unit res = func->invoke(pars);
		if (!is(res, T_VOID))
			push(ctx, res);
	}
}

void eval(context* ctx) {
	while (!ctx->input.eof) {
		switch (curr(ctx)) {
			case '\0' ... ' ': // ERROR: unrecognized character
				next(ctx);
				continue;
			case '#':
				parse_comment(ctx);
				continue;
			case '0' ... '9':
			case '-':
			case '.':
				parse_num(ctx);
				break;
			case '"':
				parse_string(ctx);
				break;
			case 'a' ... 'z':
			case 'A' ... 'Z':
			case '_':
				parse_func(ctx);
				break;
			case '(':
				next(ctx);
				unit* base = ctx->stack.base;
				ctx->stack.base = ctx->stack.top;
				eval(ctx);
				ctx->stack.base = base;
				break;
			case ')':
				next(ctx);
				return;
			default:
				parse_op(ctx);
				break;
		}
		collapse(ctx);
	}
}
