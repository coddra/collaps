#include "cuilt.h"

int test(strlist argv) {
	strlist files = FILES(config.project.test, ".laps");
	size_t passcount = 0;
	FOREACH(file, files, {
		char* buf = NULL;
		if (RUNO(&buf, OUTPUT, file) != 0) {
			ERROR("test %s failed to run", file);
			continue;
		}
		char* outf = reallocat(no_extension(file), ".out");
		char* res = read_file(outf);
		if (strcmp(buf, res) == 0) {
			INFO("test %s passed", file);
			passcount++;
		} else {
			ERROR("test %s failed", file);
		}
		free(buf);
		free(outf);
		free(res);
	});
	free_list(files);
	msg(passcount == files.count ? LOG_INFO : LOG_ERROR, "%d/%d tests passed", passcount, files.count);
	return passcount != files.count;
}

CONFIG({
	.cc = {
		.flags = LIST("-lm", "-Wall", "-Werror", "-std=c23"),
	},
	.process = {
		.test = &test
	} 
})