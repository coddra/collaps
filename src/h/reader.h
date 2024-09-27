#ifndef _READER_H
#define _READER_H

#include <stdbool.h>
#include "context.h"

#define BUF_SIZE 1024

context open(const char* path);
void close(context* ctx);

char next(context* ctx);
char peek(context* ctx);
char back(context* ctx);

static inline char curr(const context* ctx) {
    return ctx->input.eof ? '\0' : ctx->input.buf[ctx->input.pos];
}

static inline char* token_start(const context* ctx) {
    return ctx->input.buf + ctx->input.tok;
}
static inline size_t token_length(const context* ctx) {
    return ctx->input.pos - ctx->input.tok;
}
static inline void enter_token(context* ctx) {
    ctx->input.tok = ctx->input.pos;
    ctx->token_location = ctx->location;
}

#endif // _READER_H