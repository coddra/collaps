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

    location loc = {
        .file = path,
        .line = 1,
        .column = 1,
    };
    context res = {
        .input = {
            .stream = stream,
            .buf = buf,
            .tok = 0,
            .pos = 0,
            .size = BUF_SIZE,
            .eof = false,
        },
        .loc = loc,
        .tokloc = loc,
        .stack = {
            .capacity = 16,
            .count = 0,
            .items = (unit*)malloc(16 * sizeof(unit)),
        },
        .base = 0,
        .closer = '\0',
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
    if (ctx->input.buf[ctx->input.pos + 1] != '\0')
        return true;

    if (ctx->input.tok >= BUF_SIZE / 2) {
        memmove(ctx->input.buf, ctx->input.buf + ctx->input.tok, ctx->input.size - ctx->input.tok);
        ctx->input.pos -= ctx->input.tok;
        ctx->input.tok = 0;
        if (ctx->input.size > 2 * BUF_SIZE) {
            ctx->input.size -= BUF_SIZE;
            ctx->input.buf = (char*)realloc(ctx->input.buf, ctx->input.size);
        }
        if (ctx->input.stream == stdin)
            printf("> ");
        if (!fgets(ctx->input.buf + ctx->input.pos + 1, ctx->input.size - ctx->input.pos - 1, ctx->input.stream)) {
            ctx->input.eof = true;
            return false;
        }
    } else {
        ctx->input.size = ctx->input.pos + 1 + BUF_SIZE;
        ctx->input.buf = (char*)realloc(ctx->input.buf, ctx->input.size);
        if (ctx->input.stream == stdin)
            printf("> ");
        if (!fgets(ctx->input.buf + ctx->input.pos + 1, BUF_SIZE, ctx->input.stream)) {
            ctx->input.eof = true;
            return false;
        }
    }
    return true;
}

char next(context* ctx) {
    if (!need1more(ctx))
        return '\0';

    if (ctx->input.buf[ctx->input.pos] == '\n') {
        ctx->loc.line++;
        ctx->loc.column = 1;
    } else {
        ctx->loc.column++;
    }
    return ctx->input.buf[++ctx->input.pos];
}

char peek(context* ctx) {
    if (!need1more(ctx))
        return '\0';

    return ctx->input.buf[ctx->input.pos + 1];
}

char back(context* ctx) {
    assert(ctx->input.pos > ctx->input.tok);
    assert(ctx->input.buf[ctx->input.pos - 1] != '\n');

    ctx->loc.column--;
    return ctx->input.buf[--ctx->input.pos];
}
