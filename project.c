#include "cuilt.c"

int test(strlist argv) {
	const char* test_c = PATH(config.project.test, "test.c");
	const char* test_out = PATH(config.project.test, "test");
	CC(LIST(test_c), test_out);
	if (RUN(test_out) != 0)
		FATAL("build has errors");
	else
	 	INFO("test build successful");

	strlist files = FILES(config.project.test, ".laps");
	size_t passcount = 0;
	for (size_t i = 0; files[i] != NULL; i++) {
		char* buf = NULL;
		if (RUNO(&buf, output, files[i]) != 0) {
			ERROR("test %s failed to run", files[i]);
			continue;
		}
		char* outf = reallocat(no_extension(files[i]), ".out");
		char* res = read_file(outf);
		if (strcmp(buf, res) == 0) {
			INFO("test %s passed", files[i]);
			passcount++;
		} else {
			ERROR("test %s failed", files[i]);
		}
		free(buf);
		free(outf);
		free(res);
	};
	size_t filecount = length(files);
	msg(passcount == filecount ? LOG_INFO : LOG_ERROR, "%d/%d tests passed", passcount, filecount);
	return passcount != filecount;
}

CONFIG({
	.cc = {
		.flags = LIST("-lm", "-Wall", "-Werror", "-std=c23"),
	},
	.process = {
		.test = &test,
	} 
})
