#include "cuilt.h"

int test(strlist argv) {
	FOREACH(file, FILES(config.project.test, ".laps"),
		RUN(OUTPUT, file);
	);
	return 0;
}

CONFIG({
	.cc = {
		.flags = LIST("-lm", "-Wall", "-Werror", "-std=c23"),
	},
	.process = {
		.test = &test
	} 
})