#ifndef _OP_H
#define _OP_H

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
#define OP(key, name, argc, type, ...) OP_(name),
#include "builtins.h"
#undef OP
    OP_COUNT
};

extern func ops[OP_COUNT];

#define OPDEF
#include "builtins.h"

#endif
