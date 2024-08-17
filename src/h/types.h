#ifndef _TYPES_H
#define _TYPES_H

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
    T_TYPE,
    T_FIELD,
};

typedef uint64_t unit;

struct list {
    unit count;
    unit capacity;
    unit readonly;
    unit* __items;
};

struct func {
    unit name;
    unit argc;
    unit builtin;
    unit (*__invoke)(unit* args);
};

struct field {
    unit name;
    unit readonly;
};

struct type {
    unit name;
    unit fields;
};

struct list list_new();
void push(struct list* l, unit item);
void drop(struct list* l, size_t n);

unit invoke(struct func f, ...);

#endif // _TYPES_H