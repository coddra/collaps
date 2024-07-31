#ifndef _EVAL_H
#define _EVAL_H

#include "unit.h"
#include "util.h"
#include "reader.h"

#define STACK_SIZE 65536

void push(context* ctx, unit u);

void collapse(context* ctx);
void eval(context* ctx);

#endif
