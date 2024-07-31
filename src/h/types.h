#ifndef _TYPES_H
#define _TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef uint16_t type_id;
enum : type_id {
    T_VOID,
    T_INT,
    T_FLOAT,
    T_STR,
    T_LIST,
    T_FUNC,
};

typedef uint64_t unit;

typedef struct {
    unit count;
    unit capacity;
    unit* items;
} list;

typedef struct {
    const char* name;
    size_t argc;
    bool builtin;
    unit (*invoke)(unit* args);
} func;

void push(list* l, unit item);
void drop(list* l, size_t n);

#endif // _TYPES_H