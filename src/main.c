#include "h/eval.h"
#include "h/reader.h"

int main(int argc, char *argv[]) {
	context ctx;
	
	if (argc == 2)
		ctx = open(argv[1], false);
	else
		ctx = open("(stdin)", true);

	eval(&ctx);
	close(&ctx);
}

