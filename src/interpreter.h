#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include "unit.h"
#include "util.h"

#define STACK_SIZE 65536
extern unit stack[STACK_SIZE];
extern size_t sptr;

#define BUF_SIZE 2048
extern char buf[BUF_SIZE];
extern size_t bptr;

void collapse();
void eval(FILE* fp);
void ret(unit u);

#endif