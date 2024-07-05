#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"
#include "op.h"

void parse_num() {
    char* start = buf + bptr;
    char* end = start;
    uint64_t base = 10;
    bool negative = false;

    if (*start == '-') {
        negative = true;
        end++;
        if (!((*start >= '0' && *start <= '9') || *start == '.')) {
            stack[sptr++] = mkop(OP_SUB);
            bptr = start - buf;
            return;
        }
    }

    if (*end == '0') {
        end++;
        switch (*end) {
            case 'x':
                base = 24;
            case 'b':
                base -= 6;
            case 'o':
                base -= 2;
                end++;
                break;
        }
    }

    while ((*end >= '0' && *end <= (base == 2 ? '1' : base == 8 ? '7' : '9'))
            || (base == 16 && ((*end >= 'a' && *end <= 'f') || (*end >= 'A' && *end <= 'F'))))
        end++;

    bool is_float = false;
    if (*end == '.'
            || (base == 10 && (*end == 'e' || *end == 'E'))
            || (base == 16 && (*end == 'p' || *end == 'P')))
        is_float = true;

    if (is_float) {
        double val = strtod(start, &end);
        stack[sptr++] = mkfloat(val);
    } else {
        if (negative) start++;
        if (base != 10) start += 2;
        int64_t val = strtoll(start, &end, base);
        if (negative) val = -val;
        stack[sptr++] = mkint(val);
    }

    bptr = end - buf;
}

void parse_string() {
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
	bptr++;
}
