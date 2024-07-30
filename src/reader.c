#include "h/reader.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

context open(const char* path, bool isstdin) {
    FILE* stream = isstdin ? stdin : fopen(path, "r");
    if (!stream) {
        // FATAL: Cannot open file
        exit(1);
    }

    if (isstdin)
        printf("> ");
    char* buf = (char*)malloc(BUF_SIZE);
    buf = fgets(buf, BUF_SIZE, stream);

    context res = {
        .input = {
            .stream = stream,
            .buf = buf,
            .tok = buf,
            .pos = buf,
            .size = BUF_SIZE,
            .eof = false,
        },
        .loc = {
            .file = path,
            .line = 1,
            .column = 1,
        }
    };

    return res;
}

void close(context* ctx) {
    if (ctx->input.stream != stdin) 
        fclose(ctx->input.stream);
    free(ctx->input.buf);
}

bool need1more(context* ctx) {
    if (ctx->input.eof)
        return false;

    if (*(ctx->input.pos + 1) == '\0')
        ctx->input.size = ctx->input.pos - ctx->input.buf + 1;
    if (ctx->input.pos - ctx->input.buf < ctx->input.size - 1)
        return true;

    size_t diff = ctx->input.tok - ctx->input.buf;
    if (diff >= BUF_SIZE / 2) {
        memmove(ctx->input.buf, ctx->input.tok, ctx->input.size - diff);
        ctx->input.tok = ctx->input.buf;
        ctx->input.pos -= diff;
        if (diff > BUF_SIZE) {
            ctx->input.size -= BUF_SIZE;
            diff -= BUF_SIZE;
            char* tmp = ctx->input.buf;
            ctx->input.buf = (char*)realloc(ctx->input.buf, ctx->input.size);
            ctx->input.pos = ctx->input.buf + (ctx->input.pos - tmp);
            ctx->input.tok = ctx->input.buf + (ctx->input.tok - tmp);
        }
        if (ctx->input.stream == stdin)
            printf("> ");
        if (fgets(ctx->input.buf + ctx->input.size - diff, diff, ctx->input.stream) == NULL) {
            ctx->input.eof = true;
            return false;
        }
    } else {
        char* tmp = ctx->input.buf;
        ctx->input.buf = (char*)realloc(ctx->input.buf, ctx->input.size + BUF_SIZE);
        ctx->input.pos = ctx->input.buf + (ctx->input.pos - tmp);
        ctx->input.tok = ctx->input.buf + (ctx->input.tok - tmp);
        
        if (ctx->input.stream == stdin)
            printf("> ");
        if (fgets(ctx->input.buf + ctx->input.size, BUF_SIZE, ctx->input.stream) == NULL) {
            ctx->input.eof = true;
            return false;
        }
        ctx->input.size += BUF_SIZE;
    }
    return true;
}

char next(context* ctx) {
    if (!need1more(ctx))
        return '\0';

    if (*ctx->input.pos == '\n') {
        ctx->loc.line++;
        ctx->loc.column = 1;
    } else {
        ctx->loc.column++;
    }
    ctx->input.pos++;
    return *ctx->input.pos;
}

char peek(context* ctx) {
    if (!need1more(ctx))
        return '\0';

    return *(ctx->input.pos + 1);
}

char back(context* ctx) {
    assert(ctx->input.pos >= ctx->input.tok);
    assert(*(ctx->input.pos - 1) != '\n');

    ctx->loc.column--;
    return *(--ctx->input.pos);
}

char curr(const context* ctx) {
    return ctx->input.eof ? '\0' : *ctx->input.pos;
}

size_t tokenlen(const context* ctx) {
    return ctx->input.pos - ctx->input.tok;
}

void tokenstart(context* ctx) {
    ctx->input.tok = ctx->input.pos;
    ctx->tokloc = ctx->loc;
}