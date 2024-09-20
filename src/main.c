#include "h/eval.h"
#include "h/reader.h"
#include "h/context.h"
#include "h/builtins.h"

int main(int argc, char* argv[]) {
	init_builtins();
	context ctx = open(argc > 1 ? argv[1] : NULL);

	eval(&ctx);
	close(&ctx);
}

