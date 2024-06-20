#ifndef _UNIT_H
#define _UNIT_H

#include "util.h"
#include <stdint.h>
#include <string.h>

enum : uint64_t{
    T_NUM = 0b10,
    T_OP = 0b11,
    T_LIST = 0b00,
    T_STR = 0b01,

    T__INT = 0b100,
    T__FLOAT = 0b101,

    T_VOID = 0b111,
};
enum : char {
    M_VOID =    0b00000,
    M_OP =      0b00001,
    M_LIST =    0b00010,
    M_STR =     0b00100,
    M_INT =     0b01000,
    M_FLOAT =   0b10000,
    M_NUM =     M_INT | M_FLOAT,
    M_ANY =     M_OP | M_LIST | M_STR | M_NUM,
    M_ARR =     M_LIST | M_STR,
};

typedef uint64_t unit;
union convert {
    double d;
    uint64_t i;
};

#define VAL_WIDTH 62ul
#define NUM_WIDTH 61ul // this makes the smallest positive float 1.78e-306, the largest integer 1152921504606846976
#define OP_WIDTH 61ul
#define LEN_WIDTH 14ul // enough for up to 16384 items
#define PTR_WIDTH 48ul // on x86-64, user space adresses always fit on 48 bits. This might differs on other architectures
#define VAL_MASK ((1ul << VAL_WIDTH) - 1)
#define NUM_MASK ((1ul << NUM_WIDTH) - 1)
#define OP_MASK ((1ul << OP_WIDTH) - 1)
#define PTR_MASK ((1ul << PTR_WIDTH) - 1)
#define LEN_MASK ((1ul << LEN_WIDTH) - 1)
#define SIGN_MASK (1ul << (NUM_WIDTH - 1))


static inline char gettype(unit u) {
    switch ((u >> VAL_WIDTH) == T_NUM ? (u >> NUM_WIDTH) : (u >> VAL_WIDTH) == T_OP ? (u >> OP_WIDTH) : (u >> VAL_WIDTH)) {
        case T__INT: return M_INT;
        case T__FLOAT: return M_FLOAT;
        case T_LIST: return M_LIST;
        case T_STR: return M_STR;
        case T_VOID: return M_VOID;
        default: return M_OP;
    }
}
static inline char is(unit u, char type_mask) { return type_mask == M_VOID ? gettype(u) == M_VOID : gettype(u) & type_mask; }

static inline int64_t getint(unit u) { return ((u & NUM_MASK) ^ SIGN_MASK) - SIGN_MASK; }
static inline double getfloat(unit u) { 
    union convert c = { .i = (u & NUM_MASK) << (64 - NUM_WIDTH) };
    return c.d; 
}
static inline uint32_t getlen(unit u) { return (u >> PTR_WIDTH) & LEN_MASK; }
static inline char *getstr(unit u) { return (char*)(u & PTR_MASK); }
static inline unit *getlist(unit u) { return (unit*)(u & PTR_MASK); }
static inline uint64_t getop(unit u) { return u & OP_MASK; }

static inline unit mkint(int64_t i) { return (i & NUM_MASK) | (T__INT << NUM_WIDTH); }
static inline unit mkfloat(double d) {
    union convert c = { d };
    return (c.i >> (64 - NUM_WIDTH)) | (T__FLOAT << NUM_WIDTH);
}
static inline unit mkstr(char *s) { return strlen(s) << PTR_WIDTH | ((uint64_t)s & PTR_MASK) | (T_STR << VAL_WIDTH); }
static inline unit mklist(unit *l, uint32_t len) { return ((uint64_t)len << PTR_WIDTH) | ((uint64_t)l & PTR_MASK) | (T_LIST << VAL_WIDTH); }
static inline unit mkop(uint64_t index) { return (T_OP << VAL_WIDTH) | index; }
static inline unit mkvoid() { return T_VOID << OP_WIDTH; }

static inline unit as(unit u, char type) {
    switch (gettype(u)) {
        case M_INT: 
            if (type & M_INT) return u;
            else if (type & M_FLOAT) return mkfloat((double)getint(u));
            else if (type & M_STR) return mkstr(itoa(getint(u)));
            else if (type & M_LIST) return mklist(box(u), 1);
            break;
        case M_FLOAT: 
            if (type & M_FLOAT) return u;
            else if (type & M_INT) return mkint((int64_t)getfloat(u));
            else if (type & M_STR) return mkstr(ftoa(getfloat(u)));
            else if (type & M_LIST) return mklist(box(u), 1);
            break;
        case M_STR: 
            if (type & M_STR) return u;
            else if (type & M_LIST) return mklist(box(u), 1);
            break;
        case M_LIST: 
            if (type & M_LIST) return u;
            break;
    }
    return mkvoid();
}


#endif