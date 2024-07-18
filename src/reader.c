#include "h/reader.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

context open(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        // FATAL: Cannot open file
        exit(1);
    }
    char* buf = (char*)malloc(BUF_SIZE);
    buf = fgets(buf, BUF_SIZE, file);

    context res = {
        .file = file,
        .buf = buf,
        .tok = buf,
        .pos = buf,
        .size = BUF_SIZE,
        .eof = false,
    };

    return res;
}

void close(context* ctx) {
    fclose(ctx->file);
    free(ctx->buf);
}

bool need1more(context* ctx) {
    if (ctx->eof)
        return false;

    if (*(ctx->pos + 1) == '\0')
        ctx->size = ctx->pos - ctx->buf + 1;

    if (ctx->pos - ctx->buf >= ctx->size - 1) {
        size_t diff = ctx->tok - ctx->buf;
        if (diff >= BUF_SIZE / 2) {
            memmove(ctx->buf, ctx->tok, ctx->size - diff);
            ctx->tok = ctx->buf;
            ctx->pos -= diff;

            if (diff > BUF_SIZE) {
                ctx->size -= BUF_SIZE;
                diff -= BUF_SIZE;
                char* tmp = ctx->buf;
                ctx->buf = (char*)realloc(ctx->buf, ctx->size);
                ctx->pos = ctx->buf + (ctx->pos - tmp);
                ctx->tok = ctx->buf + (ctx->tok - tmp);
            }

            if (fgets(ctx->buf + ctx->size - diff, diff, ctx->file) == NULL) {
                ctx->eof = true;
                return false;
            }
        } else {
            char* tmp = ctx->buf;
            ctx->buf = (char*)realloc(ctx->buf, ctx->size + BUF_SIZE);
            ctx->pos = ctx->buf + (ctx->pos - tmp);
            ctx->tok = ctx->buf + (ctx->tok - tmp);
            if (fgets(ctx->buf + ctx->size, BUF_SIZE, ctx->file) == NULL) {
                ctx->eof = true;
                return false;
            }
            ctx->size += BUF_SIZE;
        }
    }

    return true;
}

char next(context* ctx) {
    if (!need1more(ctx))
        return '\0';

    ctx->pos++;
    return *ctx->pos;
}

char peek(context* ctx) {
    if (!need1more(ctx))
        return '\0';

    return *(ctx->pos + 1);
}

char step(context* ctx) {
    if (ctx->eof)
        return '\0';

    char res = *ctx->pos;
    if (need1more(ctx))
        ctx->pos++;

    return res;
}

char back(context* ctx) {
    assert(ctx->pos >= ctx->tok);
    return *(--ctx->pos);
}

char curr(const context* ctx) {
    return ctx->eof ? '\0' : *ctx->pos;
}

size_t tokenlen(const context* ctx) {
    return ctx->pos - ctx->tok;
}

void tokenstart(context* ctx) {
    ctx->tok = ctx->pos;
}