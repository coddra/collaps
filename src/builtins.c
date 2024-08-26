#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include "h/builtins.h"

tFunc ops[OP_COUNT] = {0};
tFunc funcs[FUNC_COUNT] = {0};

#define OP(key, name, argc, ...) unit CAT(o, name)(unit* args) __VA_ARGS__
#define FUNC(name, argc, ...) unit CAT(f, name)(unit* args) __VA_ARGS__
#   include "h/builtindefs.h"
#undef FUNC
#undef OP

void init_builtins() {
#define OP(key, name, argc, ...) ops[OP_(name)] = (tFunc){ mkstr(key), argc, true, &CAT(o, name) };
#define FUNC(name, argc, ...) funcs[FUNC_(name)] = (tFunc){ mkstr(#name), argc, true, &CAT(f, name) };
#   include "h/builtindefs.h"
#undef OP
#undef FUNC
}

tList list_new() {
    return (tList){ 0, 16, false, (unit*)malloc(16 * sizeof(unit)) };
}

void push(tList *l, unit item) {
    if (l->count == l->capacity) {
        l->capacity *= 2;
        l->__items = realloc(l->__items, l->capacity * sizeof(unit));
    }
    l->__items[l->count++] = item;
}

void drop(tList *l, size_t n) {
    l->count -= n;
    if (l->capacity > 16 && l->count < l->capacity / 4) {
        l->capacity /= 2;
        l->__items = realloc(l->__items, l->capacity * sizeof(unit));
    }
}

unit invoke(tFunc f, ...) {
    unit args[MAX_ARGC] = {0};
    va_list va;
    va_start(va, f);
    for (int i = 0; i < f.argc; i++)
        args[i] = va_arg(va, unit);
    va_end(va);
    return f.__invoke(args);
}