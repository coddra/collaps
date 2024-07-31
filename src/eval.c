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
		unit* u = ctx->stack.top - 1;
		while (ctx->stack.top - 1 - u <= MAX_ARGC && u >= ctx->stack.base && !is(*u, T_FUNC))
			u--;
		if (!is(*u, T_FUNC))
			break;

		func* func = getfunc(*u);
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

unit stack[STACK_SIZE];
void eval(context* ctx) {
	ctx->stack.top = stack;
	ctx->stack.base = stack;
	ctx->stack.origin = stack;
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
			default:
				parse_op(ctx);
				break;
		}
		collapse(ctx);
	}
}
