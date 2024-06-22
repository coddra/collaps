#ifndef _OP_H
#define _OP_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "unit.h"

#define MAX_ARGC 4

typedef struct {
    char key;
    size_t argc;
    char ret;
    void *func;
} op_t;

#define OP_(name) CAT(OP_, name)

enum {
#define OP(key, name, argc, type, ...) OP_(name),
#include "opdef.h"
#undef OP
    OP_COUNT
};

extern op_t ops[OP_COUNT];

#define OP(key, name, argc, type, ...) static inline unit CAT(FUNC_, OP_(name)) __VA_ARGS__
#include "opdef.h"
#undef OP

#endif