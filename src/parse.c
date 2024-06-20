#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void parsenum() {
	stack[sptr++] = buf[bptr++] - '0';
	while (buf[bptr] >= '0' && buf[bptr] <= '9')
		stack[sptr - 1] = stack[sptr - 1] * 10 + buf[bptr++] - '0';
	stack[sptr - 1] = mkint(stack[sptr - 1]);
    bptr--;
}

void parseString() {
	char *res = NULL;
	size_t length = 0;
	bptr++;
	size_t start = bptr;
	while (1) {
        if (bptr >= BUF_SIZE)
			break;
		if (buf[bptr] == '"' || buf[bptr] == '\\' || buf[bptr] == '\0') {
			res = (char*)realloc(res, length + bptr - start + 1);
			memcpy(res + length, buf + start, bptr - start);
			length += bptr - start;
			res[length] = '\0';
		}
		if (buf[bptr] == '"' || buf[bptr] == '\0') {
			break;
		} else if (buf[bptr] == '\\') {
			bptr++;
			start = bptr + 1;
			switch (buf[bptr]) {
#define ESCAPE_CASE(c, r) case c: res[length] = r; break
				ESCAPE_CASE('\\', '\\');
				ESCAPE_CASE('"', '"');
				ESCAPE_CASE('n', '\n');
				ESCAPE_CASE('r', '\r');
				ESCAPE_CASE('t', '\t');
				ESCAPE_CASE('b', '\b');
				ESCAPE_CASE('f', '\f');
				ESCAPE_CASE('v', '\v');
				ESCAPE_CASE('a', '\a');
				ESCAPE_CASE('e', '\e');
#undef ESCAPE_CASE
				case '0' ... '9':
					res[length] = buf[bptr] - '0';
					break;
				case 'x':
					if (bptr + 2 >= BUF_SIZE || buf[bptr + 1] == '\0' || buf[bptr + 2] == '\0')
						break;
					res[length] = (char)((fromhex(buf[bptr + 1]) << 4) + fromhex(buf[bptr + 2]));
					bptr += 2;
					start += 2;
					break;
				default:
					res[length] = buf[bptr];
					break;
			}
			length++;
		}
		bptr++;
	}
	stack[sptr++] = mkstr(res);
}
