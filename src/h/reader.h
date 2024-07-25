#ifndef _READER_H
#define _READER_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#define BUF_SIZE 1024

typedef struct {
    struct {
        FILE* stream;
        const char* name;
        char* buf;
        const char* tok;
        char* pos;
        size_t size;
        bool eof;
    } input;
} context;

context open(const char* path, bool isstdin);
void close(context* ctx);

char next(context* ctx);
char peek(context* ctx);
char step(context* ctx);
char back(context* ctx);
char curr(const context* ctx);

size_t tokenlen(const context* ctx);
void tokenstart(context* ctx);

#endif // _READER_H