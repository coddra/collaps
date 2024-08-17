#ifndef _UTIL_H
#define _UTIL_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "unit.h"

#define _CAT(a, b) a##b
#define CAT(a, b) _CAT(a, b)

static inline char *itoa(int64_t i) { 
    char *res = (char*)malloc(21);
    sprintf(res, "%ld", i);
    return res;
}
static inline char *ftoa(double d) { 
    char *res = (char*)malloc(21);
    sprintf(res, "%f", d);
    return res;
}

static inline uint64_t *box(uint64_t i) { 
    uint64_t *r = (uint64_t*)malloc(sizeof(uint64_t));
    r[0] = i;
    return r;
}

static inline bool is_digit(char c, int base) {
    return 
        (c >= '0' && c <= '9' && c - '0' < base) ||
        (c >= 'a' && c <= 'z' && c - 'a' <= base - 10) || 
        (c >= 'A' && c <= 'Z' && c - 'A' <= base - 10);
}

static inline bool is_opchar(char c) {
    switch (c) {
        case '!'...'\'':
        case '*'...'/':
        case ':'...'@':
        case '\\':
        case '^'...'`':
        case '{'...'~':
            return true;
    }
    return false;
}

static inline char fromhex(char c) {
    return (c >= '0' && c <= '9') ? c - '0' : 
        (c >= 'a' && c <= 'f') ? (c - 'a' + 10) : 
        (c >= 'A' && c <= 'F') ? (c - 'A' + 10) :
        0;
}

static inline int binsearchfunc(struct func* funcs, size_t n, const char* start, size_t length) {
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

#endif