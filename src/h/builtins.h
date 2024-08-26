#ifndef _BUILTINS_H
#define _BUILTINS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "util.h"

#define TYPE_(name) CAT(TYPE_, name)
#define OP_(name) CAT(OP_, name)
#define FUNC_(name) CAT(FUNC_, name)

enum TYPE {
#   define ZTYPE TYPE
#   define TYPE(name, ...) TYPE_(name),
#      include "builtindefs.h"
#   undef ZTYPE
#   undef TYPE
    TYPE_COUNT
};
enum OP {
#   define OP(key, name, ...) OP_(name),
#       include "builtindefs.h"
#   undef OP
    OP_COUNT
};
enum FUNC {
#   define FUNC(name, ...) FUNC_(name),
#       include "builtindefs.h"
#   undef FUNC
    FUNC_COUNT
};

#define INT_WIDTH 63ul // max 4 611 686 018 427 387 903, min -4 611 686 018 427 387 904
#define FLOAT_WIDTH 62ul // smallest positive value: 8.9e-307
#define TYPE_WIDTH 14ul // up to 16384 different types
#define PTR_WIDTH 48ul // on x86-64, user space adresses always fit on 48 bits
#define INT_MASK ((1ul << INT_WIDTH) - 1)
#define FLOAT_MASK ((1ul << FLOAT_WIDTH) - 1)
#define PTR_MASK ((1ul << PTR_WIDTH) - 1)
#define TYPE_MASK ((1ul << TYPE_WIDTH) - 1)
#define SIGN_MASK (1ul << (INT_WIDTH - 1))

#define OBJ_T (3ul << (TYPE_WIDTH + PTR_WIDTH))
#define FLOAT_T (2ul << (TYPE_WIDTH + PTR_WIDTH))

#define MAX_ARGC 16
#define OP_MAX_LENGTH 3

typedef uint64_t unit;
typedef uint16_t type_id;
union convert {
    double d;
    uint64_t i;
};

#define FIELD(name) unit name;
#define HIDDEN(...) __VA_ARGS__;
#define ZTYPE(name)
#define TYPE(name, fields) typedef struct { fields } CAT(t, name);
#   include "builtindefs.h"
#undef FIELD
#undef HIDDEN
#undef ZTYPE
#undef TYPE

static inline type_id gettype(unit u) {
    if (u >> INT_WIDTH == 0)
        return TYPE_Int;
    else if (u >> FLOAT_WIDTH == 2)
        return TYPE_Float;
    else
        return (u >> PTR_WIDTH) & TYPE_MASK;
}
static inline bool is(unit u, type_id type) { return gettype(u) == type; }
static inline bool isnull(unit u) { return (u & OBJ_T) == OBJ_T && (u & PTR_MASK) == 0; }

static inline int64_t getint(unit u) { return ((u & INT_MASK) ^ SIGN_MASK) - SIGN_MASK; }
static inline double getfloat(unit u) { 
    union convert c = { .i = (u & FLOAT_MASK) << (64 - FLOAT_WIDTH) };
    return c.d; 
}
static inline const char* getstr(unit u) { return (char*)(u & PTR_MASK); }
static inline tList* getlist(unit u) { return (tList*)(u & PTR_MASK); }
static inline tFunc* getfunc(unit u) { return (tFunc*)(u & PTR_MASK); }

static inline void* getptr(unit u) { return (void*)(u & PTR_MASK); }

static inline unit mkint(int64_t i) { return (i & INT_MASK); }
static inline unit mkfloat(double d) {
    union convert c = { d };
    return (c.i >> (64 - FLOAT_WIDTH)) | FLOAT_T;
}
static inline unit mkstr(char *s) { return ((unit)s & PTR_MASK) | ((unit)TYPE_String << PTR_WIDTH) | OBJ_T; }
static inline unit mklist(tList* l) { return ((unit)l & PTR_MASK) | ((unit)TYPE_List << PTR_WIDTH) | OBJ_T; }
static inline unit mkfunc(tFunc* f) { return ((unit)f & PTR_MASK) | ((unit)TYPE_Func << PTR_WIDTH) | OBJ_T; }
static inline unit mkvoid() { return OBJ_T; }

static inline unit as(unit u, type_id type) {
    switch (gettype(u)) {
        case TYPE_Int: 
            switch (type) {
                case TYPE_Int: return u;
                case TYPE_Float: return mkfloat((double)getint(u));
                default: return mkvoid();
            }
        case TYPE_Float:
            switch (type) {
                case TYPE_Int: return mkint((int64_t)getfloat(u));
                case TYPE_Float: return u;
                default: return mkvoid();
            }
        default:
            return mkvoid();
    }
}


#define OP(key, name, argc, ...) unit CAT(o, name)(unit* args);
#define FUNC(name, argc, ...) unit CAT(f, name)(unit* args);
#   include "builtindefs.h"
#undef OP
#undef FUNC

extern tType types[TYPE_COUNT];
extern tFunc ops[OP_COUNT];
extern tFunc funcs[FUNC_COUNT];

static inline int binsearchfunc(tFunc* funcs, size_t n, const char* start, size_t length) {
    size_t l = 0, r = n;
    int m = 1;
    while (l < r) {
        m = (l + r) / 2;
        int cmp = strncmp(getstr(funcs[m].name), start, length);
        if (cmp == 0)
            cmp = length < strlen(getstr(funcs[m].name));
        if (cmp < 0) l = m + 1;
        else if (cmp > 0) r = m;
        else return m;
    }
    return -m - 1;
}

void init_builtins();

// TODO: Methods
tList list_new();
void push(tList* l, unit item);
void drop(tList* l, size_t n);
unit invoke(tFunc f, ...);

#endif
