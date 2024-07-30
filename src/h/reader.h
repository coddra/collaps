#ifndef _READER_H
#define _READER_H

#include <stdbool.h>
#include "context.h"

#define BUF_SIZE 1024

context open(const char* path, bool isstdin);
void close(context* ctx);

char next(context* ctx);
char peek(context* ctx);
char back(context* ctx);
char curr(const context* ctx);

size_t tokenlen(const context* ctx);
void tokenstart(context* ctx);

#endif // _READER_H