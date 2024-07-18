#include "h/eval.h"
#include "h/op.h"
#include "h/parse.h"

size_t sptr = 0;
unit stack[STACK_SIZE];

size_t bptr = 0;
char buf[BUF_SIZE];

func ops[OP_COUNT] = {
#define OP(key, name, argc, ...) [OP_(name)] = { key, argc, true, &CAT(FUNC_, OP_(name)) },
#include "h/builtins.h"
#undef OP
};


void push(unit u) {
	if (sptr >= STACK_SIZE) {
		fprintf(stderr, "Fatal: Stack overflow\n");
		return;
	}

	stack[sptr++] = u;
}


void collapse() {
	while (1) {
		func *top_func = NULL;
		for (int i = 1; i <= MAX_ARGC + 1 && i <= sptr && top_func == NULL; i++) 
			if (is(stack[sptr - i], T_FUNC))
				top_func = getfunc(stack[sptr - i]);
		if (top_func == NULL)
			break;
		unit pars[MAX_ARGC];
		int p = top_func->argc - 1;
		char op = 0;
		for (int i = 1; i <= top_func->argc + 1 && i <= sptr && (!op || !is(stack[sptr - i], T_FUNC)); i++) {
			if (is(stack[sptr - i], T_FUNC))
				op = 1;
			else
				pars[p--] = stack[sptr - i];
		}
		if (p != -1)
			break;
		sptr -= top_func->argc + 1;
		top_func->invoke(pars);
	}
}

void eval(context* ctx) {
	while (!ctx->eof) {
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
		collapse();
	}
}
