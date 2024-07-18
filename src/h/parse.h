#ifndef _PARSE_H
#define _PARSE_H

#include "reader.h"

void parse_comment(context* ctx);
void parse_num(context* ctx);
void parse_string(context* ctx);
void parse_op(context* ctx);
void parse_func(context* ctx);

#endif