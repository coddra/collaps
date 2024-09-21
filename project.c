#include "cuilt.c"

int test(strlist argv) {
	const char* test_c = PATH(config.project.test, "test.c");
	const char* test_out = PATH(config.project.test, "test");
	CC(LIST(test_c), test_out);
	if (CMD(test_out) != 0)
		FATAL("build has errors");
	else
	 	INFO("test build successful");

	strlist files = FILES(config.project.test, ".laps");
	size_t passcount = 0;
	size_t filecount = 0;
	for (; files[filecount] != NULL; filecount++) {
		char* outf = reallocat(no_extension(files[filecount]), ".out");
		if (TESTF(outf, output, files[filecount]) == 0) {
			INFO("test %s passed", files[filecount]);
			passcount++;
		} else {
			ERROR("test %s failed", files[filecount]);
		}
	};
	msg(passcount == filecount ? LOG_INFO : LOG_ERROR, "%d/%d tests passed", passcount, filecount);
	return passcount != filecount;
}

CONFIG({
	.cc = {
		.flags = LIST("-lm", "-Wall", "-Werror", "-std=c23", "-Wno-unused-local-typedefs", "-Wno-varargs"),
	},
	.process = {
		.test = &test,
	} 
})
