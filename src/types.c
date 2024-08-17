#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include "h/types.h"
#include "h/builtins.h"

struct list list_new() {
    return (struct list){ 0, 16, false, (unit*)malloc(16 * sizeof(unit)) };
}

void push(struct list *l, unit item) {
    if (l->count == l->capacity) {
        l->capacity *= 2;
        l->__items = realloc(l->__items, l->capacity * sizeof(unit));
    }
    l->__items[l->count++] = item;
}

void drop(struct list *l, size_t n) {
    l->count -= n;
    if (l->capacity > 16 && l->count < l->capacity / 4) {
        l->capacity /= 2;
        l->__items = realloc(l->__items, l->capacity * sizeof(unit));
    }
}

unit invoke(struct func f, ...) {
    unit args[MAX_ARGC] = {0};
    va_list va;
    va_start(va, f);
    for (int i = 0; i < f.argc; i++)
        args[i] = va_arg(va, unit);
    va_end(va);
    return f.__invoke(args);
}