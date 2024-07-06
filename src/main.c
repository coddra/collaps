#include <stdio.h>
#include <string.h>
#include "eval.h"

int main(int argc, char *argv[]) {
	FILE *fp;
	
	if (argc == 2) {
		fp = fopen(argv[1], "r");
		if (fp == NULL) {
			fprintf(stderr, "FATAL: Cannot open file %s\n", argv[1]);
			return 1;
		}
	} else {
		fp = stdin;
	}

	eval(fp);
}

