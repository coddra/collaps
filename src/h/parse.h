#ifndef _PARSE_H
#define _PARSE_H

#include "context.h"
#include "reader.h"

void parse_comment(context* ctx);
void parse_bracket(context* ctx);
unit parse_num(context* ctx);
unit parse_string(context* ctx);
unit parse_op(context* ctx);
unit parse_symbol(context* ctx);

#endif