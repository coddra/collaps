#include "cuilt.h"

int test(strlist argv) {
	FOREACH(file, FILES(config.project.test, ".laps"),
		RUN(OUTPUT, file);
	);
	return 0;
}

CONFIG({
	.cc = {
		.flags = LIST("-lm", "-Wall", "-Wpedantic", "-Werror", "-std=c11"),
	},
	.process = {
		.test = &test
	} 
})