#include "h/builtins.h"

struct func ops[OP_COUNT] = {0};
struct func funcs[FUNC_COUNT] = {0};

#define OP(key, name, argc, ...) unit CAT(o, name)(unit* args) __VA_ARGS__
#define FUNC(name, argc, ...) unit CAT(f, name)(unit* args) __VA_ARGS__
#   include "h/builtindefs.h"
#undef FUNC
#undef OP

void init_builtins() {
#define OP(key, name, argc, ...) ops[OP_(name)] = (struct func){ mkstr(key), argc, true, &CAT(o, name) };
#define FUNC(name, argc, ...) funcs[FUNC_(name)] = (struct func){ mkstr(#name), argc, true, &CAT(f, name) };
#   include "h/builtindefs.h"
#undef OP
#undef FUNC
}