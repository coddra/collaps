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

void eval(FILE *fp) {
	fgets(buf, BUF_SIZE, fp);
	while (1) {
		if (buf[bptr] == '\0' || bptr >= BUF_SIZE / 2) {
			memcpy(buf, buf + bptr, BUF_SIZE - bptr);
			bptr = 0;
			size_t buflen = strlen(buf);
			if (fgets(buf + buflen, BUF_SIZE - buflen, fp) == NULL)
				break;
		}

		switch (buf[bptr]) {
			case '\0' ... ' ':
				bptr++;
				continue;
			case '#':
				parse_comment();
				continue;
			case '0' ... '9':
			case '-':
			case '.':
				parse_num();
				break;
			case '"':
				parse_string();
				break;
			case 'a' ... 'z':
			case 'A' ... 'Z':
			case '_':
				parse_func();
				break;
			default:
				parse_op();
				break;
		}
		if (sptr >= STACK_SIZE) {
			fprintf(stderr, "Fatal: Stack overflow\n");
			return;
		}

		collapse();
	}
}

void ret(unit u) {
	stack[sptr++] = u;
}
