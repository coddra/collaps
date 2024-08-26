#ifndef _CONTEXT_H
#define _CONTEXT_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include "builtins.h"

typedef struct { 
    const char* file;
    size_t line;
    size_t column;
} location;

typedef struct {
    struct {
        FILE* stream;
        char* buf;
        size_t tok;
        size_t pos;
        size_t size;
        bool eof;
    } input;
    location loc;
    location tokloc;
    tList stack;
    size_t base;
    char closer;
} context;

static inline size_t stacksize(context* ctx) { return ctx->stack.count - ctx->base; }
static inline unit* stacktop(context* ctx) { return (unit*)ctx->stack.__items + ctx->stack.count - 1; }
static inline unit* stackidx(context* ctx, size_t i) { return (unit*)ctx->stack.__items + ctx->stack.count - 1 - i; }

#endif