#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include "unit.h"

typedef struct { 
    const char* file;
    size_t line;
    size_t column;
} location;

typedef struct {
    struct {
        FILE* stream;
        char* buf;
        const char* tok;
        char* pos;
        size_t size;
        bool eof;
    } input;
    location loc;
    location tokloc;
    struct {
        unit* origin;
        unit* base;
        unit* top;
    } stack;
} context;

static inline size_t stacksize(context* ctx) { return ctx->stack.top - ctx->stack.base; }
