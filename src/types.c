#include <stdlib.h>
#include "h/types.h"

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