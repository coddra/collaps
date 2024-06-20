#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "unit.h"

void eval(FILE *fp);

#define STACK_SIZE 65536
extern unit stack[STACK_SIZE];
extern size_t sptr;

#define BUF_SIZE 2048
extern char buf[BUF_SIZE];
extern size_t bptr;

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
#include "ops.h"
#undef OP
    OP_COUNT
};

extern op_t ops[OP_COUNT];

#define OP(key, name, argc, type, ...) static inline unit CAT(FUNC_, OP_(name)) __VA_ARGS__
#include "ops.h"
#undef OP

#endif