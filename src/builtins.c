#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include "h/builtins.h"
#include "h/context.h"


unit make(enum TYPE type, ...) {
    if (type == TYPE_Undefined)
        return ((unit)TYPE_Undefined << PTR_WIDTH) | OBJ_T;
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

enum TYPE get_typeid(unit u) {
    if (u >> INT_WIDTH == 0)
        return TYPE_Int;
    else if (u >> FLOAT_WIDTH == 2)
        return TYPE_Float;
    else
        return (u >> PTR_WIDTH) & TYPE_MASK;
}

bool is(unit u, enum TYPE type) { 
    enum TYPE t = get_typeid(u);
    switch (t) {
        case TYPE_Bool:
        case TYPE_Int:
        case TYPE_Float:
        case TYPE_String:
        case TYPE_Symbol:
        case TYPE_Undefined:
            return t == type;
        default:
            return t == type || type == TYPE_Object;
    }
}
bool is_null(unit u) { return (u & OBJ_T) == OBJ_T && (u & PTR_MASK) == 0; }

bool get_bool(unit u) { return u & PTR_MASK; }
int64_t get_int(unit u) { return ((u & INT_MASK) ^ SIGN_MASK) - SIGN_MASK; }
double get_float(unit u) { 
    union convert c = { .i = (u & FLOAT_MASK) << (64 - FLOAT_WIDTH) };
    return c.d; 
}
const char* get_str(unit u) { return (char*)(u & PTR_MASK); }

unit as(unit u, enum TYPE type) {
    switch (get_typeid(u)) {
        case TYPE_Int: 
            switch (type) {
                case TYPE_Int: return u;
                case TYPE_Float: return make_float((double)get_int(u));
                default: return make(TYPE_Undefined);
            }
        case TYPE_Float:
            switch (type) {
                case TYPE_Int: return make_int((int64_t)get_float(u));
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

#define OP(key, name, argc, ...) unit CAT(o, name)(context* ctx, unit* args) __VA_ARGS__
#define FUNC(name, argc, ...) unit CAT(f, name)(context* ctx, unit* args) __VA_ARGS__
#define ABBREVS
#   include "h/builtindefs.h"
#undef FUNC
#undef OP
#undef ABBREVS

void init_builtins() {
    enum TYPE ct;
#define FLD(name, readonly) push(get(tList*, types[ct].fields), mkfieldalloc((tField){ make(TYPE_Type, &types[TYPE_Field]), make(TYPE_String, #name), make(TYPE_Bool, readonly) }));
#define HIDDEN(...)
#define ATYPE(name, base) ZTYPE(name)
#define ZTYPE(name) types[TYPE_(name)] = (tType){ make(TYPE_Type, &types[TYPE_Type]), make(TYPE_Undefined), mklistalloc(list_new()) };
#define TYPE(name, parent, flds) types[ct = TYPE_(name)] = (tType){ make(TYPE_Type, &types[TYPE_Type]), make(TYPE_Type, &types[TYPE_(parent)]), mklistalloc(list_new()) }; \
    push(get(tList*, types[ct].fields), mkfieldalloc((tField){ make(TYPE_Type, &types[TYPE_Field]), make(TYPE_String, "__type"), make(TYPE_Bool, true) })); \
    flds
#define OP(key, name, argc, ...) ops[OP_(name)] = (tFunc){ make(TYPE_Type, &types[TYPE_Func]), make_int(argc), make(TYPE_Bool, true), &CAT(o, name) };
#define FUNC(name, argc, ...) funcs[FUNC_(name)] = (tFunc){ make(TYPE_Type, &types[TYPE_Func]), make_int(argc), make(TYPE_Bool, true), &CAT(f, name) };
#   include "h/builtindefs.h"
#undef FLD
#undef HIDDEN
#undef ATYPE
#undef ZTYPE
#undef TYPE
#undef OP
#undef FUNC
}

// TODO: Methods
tList list_new() {
    return (tList){ make(TYPE_Type, &types[TYPE_List]), 0, 16, false, (unit*)malloc(16 * sizeof(unit)) };
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

unit invoke(context* ctx, tFunc f, ...) {
    unit args[MAX_ARGC] = {0};
    va_list va;
    va_start(va, f);
    for (int i = 0; i < f.argc; i++)
        args[i] = va_arg(va, unit);
    va_end(va);
    return f.__invoke(ctx, args);
}