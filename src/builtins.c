#include "h/builtins.h"

struct func ops[OP_COUNT] = {0};
struct func funcs[FUNC_COUNT] = {0};

void init_builtins() {
#define OP(key, name, argc, ...) ops[OP_(name)] = (struct func){ mkstr(key), argc, true, &CAT(OP_(name), _DEF) };
#define FUNC(name, argc, ...) funcs[FUNC_(name)] = (struct func){ mkstr(#name), argc, true, &CAT(FUNC_(name), _DEF) };
#include "h/builtindefs.h"
#undef OP
#undef FUNC
}