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

enum {
#define OP(key, name, argc, ...) OP_(name),
#include "builtindefs.h"
#undef OP
    OP_COUNT
};
extern struct func ops[OP_COUNT];

#define FUNC_(name) CAT(FUNC_, name)
enum {
#define FUNC(name, argc, ...) FUNC_(name),
#include "builtindefs.h"
#undef FUNC
    FUNC_COUNT
};
extern struct func funcs[FUNC_COUNT];

#define OPDEF
#define FUNCDEF
#define TYPEDEF
#include "builtindefs.h"

void init_builtins();

#endif
