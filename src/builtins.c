#include "h/builtins.h"

func ops[OP_COUNT] = {
#define OP(key, name, argc, ...) [OP_(name)] = { key, argc, true, &CAT(OP_(name), _DEF) },
#include "h/builtindefs.h"
#undef OP
};

func funcs[FUNC_COUNT] = {
#define FUNC(name, argc, ...) [FUNC_(name)] = { #name, argc, true, &CAT(FUNC_(name), _DEF) },
#include "h/builtindefs.h"
#undef FUNC
};