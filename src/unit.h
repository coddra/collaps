#ifndef _UNIT_H
#define _UNIT_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

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
    unit* items;
} list;

typedef struct {
    const char* name;
    size_t argc;
    bool builtin;
    void (*invoke)(unit* args);
} func;

union convert {
    double d;
    uint64_t i;
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


static inline type_id gettype(unit u) {
    if (u >> INT_WIDTH == 0)
        return T_INT;
    else if (u >> FLOAT_WIDTH == 2)
        return T_FLOAT;
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
static inline list* getlist(unit u) { return (list*)(u & PTR_MASK); }
static inline func* getfunc(unit u) { return (func*)(u & PTR_MASK); }

static inline void* getptr(unit u) { return (void*)(u & PTR_MASK); }

static inline unit mkint(int64_t i) { return (i & INT_MASK); }
static inline unit mkfloat(double d) {
    union convert c = { d };
    return (c.i >> (64 - FLOAT_WIDTH)) | FLOAT_T;
}
static inline unit mkstr(char *s) { return ((uint64_t)s & PTR_MASK) | ((uint64_t)T_STR << PTR_WIDTH) | OBJ_T; }
static inline unit mklist(list* l) { return ((uint64_t)l & PTR_MASK) | ((uint64_t)T_LIST << PTR_WIDTH) | OBJ_T; }
static inline unit mkfunc(func* f) { return ((uint64_t)f & PTR_MASK) | ((uint64_t)T_FUNC << PTR_WIDTH) | OBJ_T; }
static inline unit mkvoid() { return OBJ_T; }

static inline unit as(unit u, type_id type) {
    switch (gettype(u)) {
        case T_INT: 
            switch (type) {
                case T_INT: return u;
                case T_FLOAT: return mkfloat((double)getint(u));
                default: return mkvoid();
            }
        case T_FLOAT:
            switch (type) {
                case T_INT: return mkint((int64_t)getfloat(u));
                case T_FLOAT: return u;
                default: return mkvoid();
            }
        default:
            return mkvoid();
    }
}

#endif
