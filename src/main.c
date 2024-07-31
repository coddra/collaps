#include "h/eval.h"
#include "h/reader.h"

unit stack[STACK_SIZE];
int main(int argc, char *argv[]) {
	context ctx;
	
	if (argc == 2)
		ctx = open(argv[1], false);
	else
		ctx = open("(stdin)", true);


	ctx.stack.top = stack;
	ctx.stack.base = stack;
	ctx.stack.origin = stack;
	eval(&ctx);
	close(&ctx);
}

