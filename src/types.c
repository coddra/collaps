#include <stdarg.h>
#include <stdlib.h>
#include "h/types.h"
#include "h/op.h"

list list_new() {
    return (list){ 0, 16, (unit*)malloc(16 * sizeof(unit)) };
}

void push(list *l, unit item) {
    if (l->count == l->capacity) {
        l->capacity *= 2;
        l->items = realloc(l->items, l->capacity * sizeof(unit));
    }
    l->items[l->count++] = item;
}

void drop(list *l, size_t n) {
    l->count -= n;
    if (l->capacity > 16 && l->count < l->capacity / 4) {
        l->capacity /= 2;
        l->items = realloc(l->items, l->capacity * sizeof(unit));
    }
}

unit invoke(func f, ...) {
    unit args[MAX_ARGC] = {0};
    va_list va;
    va_start(va, f);
    for (int i = 0; i < f.argc; i++)
        args[i] = va_arg(va, unit);
    va_end(va);
    return f.invoke(args);
}