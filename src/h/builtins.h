#ifndef _BUILTINS_H
#define _BUILTINS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "eval.h"
#include "unit.h"

#define MAX_ARGC 16
#define OP_MAX_LENGTH 3

#define OP_(name) CAT(OP_, name)
#define FUNC_(name) CAT(FUNC_, name)
#define TYPE_(name) CAT(TYPE_, name)

#define OP(key, name, argc, ...) unit CAT(o, name)(unit* args);
#define FUNC(name, argc, ...) unit CAT(f, name)(unit* args);
#define FIELD(name) unit name;
#define HIDDEN(...) __VA_ARGS__;
#define ZTYPE(name)
#define TYPE(name, fields) typedef struct { fields } CAT(t, name);
#   include "builtindefs.h"
#undef OP
#undef FUNC
#undef FIELD
#undef HIDDEN
#undef ZTYPE
#undef TYPE

enum {
#   define OP(key, name, ...) OP_(name),
#       include "builtindefs.h"
#   undef OP
    OP_COUNT
};
extern struct func ops[OP_COUNT];

enum {
#   define FUNC(name, ...) FUNC_(name),
#       include "builtindefs.h"
#   undef FUNC
    FUNC_COUNT
};
extern struct func funcs[FUNC_COUNT];

enum {
#   define ZTYPE TYPE
#   define TYPE(name, ...) TYPE_(name),
#      include "builtindefs.h"
#   undef ZTYPE
#   undef TYPE
    TYPE_COUNT
};

void init_builtins();
#endif
