#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include "h/builtins.h"

enum TYPE gettypeid(unit u) {
    if (u >> INT_WIDTH == 0)
        return TYPE_Int;
    else if (u >> FLOAT_WIDTH == 2)
        return TYPE_Float;
    else
        return (u >> PTR_WIDTH) & TYPE_MASK;
}
bool is(unit u, enum TYPE type) { 
    enum TYPE t = gettypeid(u);
    while (t != TYPE_Object && t != TYPE_Undefined && t != type)
        t = get(tType*, types[t].parent) - types;
    return t == type;
}
bool isnull(unit u) { return (u & OBJ_T) == OBJ_T && (u & PTR_MASK) == 0; }

bool getbool(unit u) { return u & PTR_MASK; }
int64_t getint(unit u) { return ((u & INT_MASK) ^ SIGN_MASK) - SIGN_MASK; }
double getfloat(unit u) { 
    union convert c = { .i = (u & FLOAT_MASK) << (64 - FLOAT_WIDTH) };
    return c.d; 
}
const char* getstr(unit u) { return (char*)(u & PTR_MASK); }

unit make(enum TYPE type, ...) {
    if (type == TYPE_Undefined)
        return OBJ_T;
    va_list va;
    va_start(va, 1);
    unit res;
    union convert c;
    switch (type) {
        case TYPE_Bool: res = va_arg(va, int) | ((unit)type << PTR_WIDTH) | OBJ_T; break;
        case TYPE_Int: res = va_arg(va, int64_t) & INT_MASK; break;
        case TYPE_Float: 
            c.d = va_arg(va, double);
            res = (c.i >> (64 - FLOAT_WIDTH)) | FLOAT_T; 
            break;
        default: res = ((unit)va_arg(va, void*) & PTR_MASK) | ((unit)type << PTR_WIDTH) | OBJ_T; break;
    }
    va_end(va);
    return res;
}
unit mklistalloc(tList l) { 
    tList* lp = (tList*)malloc(sizeof(tList));
    *lp = l;
    return make(TYPE_List, lp);
}
unit mkfieldalloc(tField f) {
    tField* fp = (tField*)malloc(sizeof(tField));
    *fp = f;
    return make(TYPE_Field, fp);
}

unit as(unit u, enum TYPE type) {
    switch (gettypeid(u)) {
        case TYPE_Int: 
            switch (type) {
                case TYPE_Int: return u;
                case TYPE_Float: return mkfloat((double)getint(u));
                default: return make(TYPE_Undefined);
            }
        case TYPE_Float:
            switch (type) {
                case TYPE_Int: return mkint((int64_t)getfloat(u));
                case TYPE_Float: return u;
                default: return make(TYPE_Undefined);
            }
        default:
            return make(TYPE_Undefined);
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
#undef ABBREVS

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
#define FLD(name, readonly) push(get(tList*, types[ct].fields), mkfieldalloc((tField){ make(TYPE_String, #name), make(TYPE_Bool, readonly) }));
#define HIDDEN(...)
#define ZTYPE(name) types[TYPE_(name)] = (tType){ make(TYPE_String, #name), make(TYPE_Undefined), mklistalloc(list_new()) };
#define TYPE(name, parent, flds) types[ct = TYPE_(name)] = (tType){ make(TYPE_String, #name), make(TYPE_Type, &types[TYPE_(parent)]), mklistalloc(list_new()) }; flds
#define OP(key, name, argc, ...) ops[OP_(name)] = (tFunc){ make(TYPE_String, key), mkint(argc), make(TYPE_Bool, true), &CAT(o, name) };
#define FUNC(name, argc, ...) funcs[FUNC_(name)] = (tFunc){ make(TYPE_String, #name), mkint(argc), make(TYPE_Bool, true), &CAT(f, name) };
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