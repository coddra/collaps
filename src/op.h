#ifndef _OP_H
#define _OP_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "interpreter.h"
#include "unit.h"

#define MAX_ARGC 4

typedef struct {
    char key;
    size_t argc;
    char ret;
    void (*func)(unit* args);
} op_t;

#define OP_(name) CAT(OP_, name)

enum {
#define OP(key, name, argc, type, ...) OP_(name),
#include "opdef.h"
#undef OP
    OP_COUNT
};

extern op_t ops[OP_COUNT];

#define OP(key, name, argc, type, ...) static inline void CAT(FUNC_, OP_(name))(unit* args) __VA_ARGS__
#define OPDEF
#include "opdef.h"
#undef OPDEF
#undef OP

#endif