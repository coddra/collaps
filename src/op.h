#ifndef _OP_H
#define _OP_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "interpreter.h"
#include "unit.h"

#define MAX_ARGC 16

#define OP_(name) CAT(OP_, name)

enum {
#define OP(key, name, argc, type, ...) OP_(name),
#include "opdef.h"
#undef OP
    OP_COUNT
};

extern func ops[OP_COUNT];

#define OP(key, name, argc, ...) static inline void CAT(FUNC_, OP_(name))(unit* args) __VA_ARGS__
#define OPDEF
#include "opdef.h"
#undef OPDEF
#undef OP

#endif
