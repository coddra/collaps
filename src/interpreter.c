#include "interpreter.h"
#include "op.h"
#include "parse.h"

size_t sptr = 0;
unit stack[STACK_SIZE];

size_t bptr = 0;
char buf[BUF_SIZE];

op_t ops[OP_COUNT] = {
#define OP(key, name, argc, type, ...) [OP_(name)] = { key, argc, type, &CAT(FUNC_, OP_(name)) },
#include "opdef.h"
#undef OP
};

void collapse() {
	while (1) {
		op_t *top_op = NULL;
		for (int i = 1; i <= MAX_ARGC + 1 && i <= sptr && top_op == NULL; i++) 
			if (is(stack[sptr - i], M_OP))
				top_op = &ops[getop(stack[sptr - i])];
		if (top_op == NULL)
			break;
		unit pars[MAX_ARGC];
		int p = top_op->argc - 1;
		char op = 0;
		for (int i = 1; i <= top_op->argc + 1 && i <= sptr && (!op || !is(stack[sptr - i], M_OP)); i++) {
			if (is(stack[sptr - i], M_OP))
				op = 1;
			else
				pars[p--] = stack[sptr - i];
		}
		if (p != -1)
			break;
		sptr -= top_op->argc + 1;
		top_op->func(pars);
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
			case '0' ... '9':
				parse_num();
				break;
			case '"':
				parse_string();
				break;
#define OP(key, name, argc, type, ...) case key: stack[sptr++] = mkop(OP_(name)); break;
#include "opdef.h"
#undef OP
		}
		if (sptr >= STACK_SIZE) {
			fprintf(stderr, "Fatal: Stack overflow\n");
			return;
		}

		collapse();
		bptr++;
	}
}

void ret(unit u) {
	stack[sptr++] = u;
}