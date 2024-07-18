#ifndef _EVAL_H
#define _EVAL_H

#include "unit.h"
#include "util.h"
#include "reader.h"

#define STACK_SIZE 65536
extern unit stack[STACK_SIZE];
extern size_t sptr;

void push(unit u);

void collapse();
void eval(context* ctx);

#endif
