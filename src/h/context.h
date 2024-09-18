#ifndef _CONTEXT_H
#define _CONTEXT_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include "builtins.h"

typedef struct context context;
struct context {
    context* parent;
    struct {
        FILE* stream;
        char* buf;
        size_t tok;
        size_t pos;
        size_t size;
        bool eof;
    } input;
    tLocation location;
    tLocation tokenLocation;
    tList stack;
    size_t base;
    char closer;
};

static inline size_t stacksize(context* ctx) { return ctx->stack.count - ctx->base; }
static inline unit* stackidx(context* ctx, size_t i) { return (unit*)ctx->stack.__items + ctx->stack.count - 1 - i; }

#endif