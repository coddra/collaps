#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include "h/builtins.h"

type_id gettype(unit u) {
    if (u >> INT_WIDTH == 0)
        return TYPE_Int;
    else if (u >> FLOAT_WIDTH == 2)
        return TYPE_Float;
    else
        return (u >> PTR_WIDTH) & TYPE_MASK;
}
bool is(unit u, type_id type) { return gettype(u) == type; }
bool isnull(unit u) { return (u & OBJ_T) == OBJ_T && (u & PTR_MASK) == 0; }

bool getbool(unit u) { return u & PTR_MASK; }
int64_t getint(unit u) { return ((u & INT_MASK) ^ SIGN_MASK) - SIGN_MASK; }
double getfloat(unit u) { 
    union convert c = { .i = (u & FLOAT_MASK) << (64 - FLOAT_WIDTH) };
    return c.d; 
}
const char* getstr(unit u) { return (char*)(u & PTR_MASK); }
tList* getlist(unit u) { return (tList*)(u & PTR_MASK); }
tFunc* getfunc(unit u) { return (tFunc*)(u & PTR_MASK); }

void* getptr(unit u) { return (void*)(u & PTR_MASK); }

unit mkbool(bool b) { return OBJ_T | ((unit)TYPE_Bool << PTR_WIDTH) | b; }
unit mkint(int64_t i) { return (i & INT_MASK); }
unit mkfloat(double d) {
    union convert c = { d };
    return (c.i >> (64 - FLOAT_WIDTH)) | FLOAT_T;
}
unit mkstr(char* s) { return ((unit)s & PTR_MASK) | ((unit)TYPE_String << PTR_WIDTH) | OBJ_T; }
unit mklist(tList* l) { return ((unit)l & PTR_MASK) | ((unit)TYPE_List << PTR_WIDTH) | OBJ_T; }
unit mklistalloc(tList l) { 
    tList* lp = (tList*)malloc(sizeof(tList));
    *lp = l;
    return mklist(lp);
}
unit mkfunc(tFunc* f) { return ((unit)f & PTR_MASK) | ((unit)TYPE_Func << PTR_WIDTH) | OBJ_T; }
unit mkfield(tField* f) { return ((unit)f & PTR_MASK) | ((unit)TYPE_Field << PTR_WIDTH) | OBJ_T; }
unit mkfieldalloc(tField f) {
    tField* fp = (tField*)malloc(sizeof(tField));
    *fp = f;
    return mkfield(fp);
}
unit mkvoid() { return OBJ_T; }

unit as(unit u, type_id type) {
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

tFunc ops[OP_COUNT] = {0};
tFunc funcs[FUNC_COUNT] = {0};
tType types[TYPE_COUNT] = {0};

#define OP(key, name, argc, ...) unit CAT(o, name)(unit* args) __VA_ARGS__
#define FUNC(name, argc, ...) unit CAT(f, name)(unit* args) __VA_ARGS__
#define ABBREVS
#   include "h/builtindefs.h"
#undef FUNC
#undef OP

int binsearchfunc(tFunc* funcs, size_t n, const char* start, size_t length) {
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

void init_builtins() {
    enum TYPE ct;
#define FLD(name, readonly) push(getlist(types[ct].fields), mkfieldalloc((tField){ mkstr(#name), mkint(readonly) }));
#define HIDDEN(...)
#define ZTYPE(name) types[TYPE_(name)] = (tType){ mkstr(#name), mklistalloc(list_new()) };
#define TYPE(name, flds) types[ct = TYPE_(name)] = (tType){ mkstr(#name), mklistalloc(list_new()) }; flds
#define OP(key, name, argc, ...) ops[OP_(name)] = (tFunc){ mkstr(key), mkint(argc), mkint(true), &CAT(o, name) };
#define FUNC(name, argc, ...) funcs[FUNC_(name)] = (tFunc){ mkstr(#name), mkint(argc), mkint(true), &CAT(f, name) };
#   include "h/builtindefs.h"
#undef OP
#undef FUNC
}


// TODO: Methods
tList list_new() {
    return (tList){ 0, 16, false, (unit*)malloc(16 * sizeof(unit)) };
}

void push(tList* l, unit item) {
    if (l->count == l->capacity) {
        l->capacity *= 2;
        l->__items = realloc(l->__items, l->capacity * sizeof(unit));
    }
    l->__items[l->count++] = item;
}

void drop(tList* l, size_t n) {
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