#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include "h/builtins.h"
#include "h/context.h"


unit make(enum TYPE type, uc u) {
    if (type == TYPE_Undefined)
        return ((unit)TYPE_Undefined << PTR_WIDTH) | OBJ_T;
    switch (type) {
        case TYPE_Bool: return u.b | ((unit)type << PTR_WIDTH) | OBJ_T;
        case TYPE_Int: return u.i & INT_MASK;
        case TYPE_Float: return (u.u >> (64 - FLOAT_WIDTH)) | FLOAT_T; 
        default:
            return ((unit)u.p & PTR_MASK) | ((unit)type << PTR_WIDTH) | OBJ_T;
    }
}

unit mklistalloc(tList l) { 
    tList* lp = (tList*)malloc(sizeof(tList));
    *lp = l;
    return make(TYPE_List, (uc){ .p = lp });
}
unit mkfieldalloc(tField f) {
    tField* fp = (tField*)malloc(sizeof(tField));
    *fp = f;
    return make(TYPE_Field, (uc){ .p = fp });
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
    uc c = { .u = (u & FLOAT_MASK) << (64 - FLOAT_WIDTH) };
    return c.d; 
}
const char* get_str(unit u) { return (char*)(u & PTR_MASK); }

unit as(unit u, enum TYPE type) {
    switch (get_typeid(u)) {
        case TYPE_Int: 
            switch (type) {
                case TYPE_Int: return u;
                case TYPE_Float: return make(TYPE_Float, (uc){ .d = get_int(u) });
                default: return vUndefined;
            }
        case TYPE_Float:
            switch (type) {
                case TYPE_Int: return make(TYPE_Int, (uc){ .i = (int64_t)get_float(u) });
                case TYPE_Float: return u;
                default: return vUndefined;
            }
        default:
            return vUndefined;
    }
}

tFunc ops[OP_COUNT] = {0};
tFunc funcs[FUNC_COUNT] = {0};
tType types[TYPE_COUNT] = {0};

const unit vUndefined = ((unit)TYPE_Undefined << PTR_WIDTH) | OBJ_T;
const unit vTrue = ((unit)TYPE_Bool << PTR_WIDTH) | OBJ_T | 1;
const unit vFalse = ((unit)TYPE_Bool << PTR_WIDTH) | OBJ_T | 0;

#define OP(key, name, argc, ...) unit CAT(o, name)(context* ctx, unit* args) __VA_ARGS__
#define FUNC(name, argc, ...) unit CAT(f, name)(context* ctx, unit* args) __VA_ARGS__
#define ABBREVS
#   include "h/builtindefs.h"
#undef FUNC
#undef OP
#undef ABBREVS

void init_builtins() {
    enum TYPE ct;
#define FLD(name, readonly) push(get(tList*, types[ct].fields), mkfieldalloc((tField){ \
        make(TYPE_Type, (uc){ .p = &types[TYPE_Field] }), \
        make(TYPE_String, (uc){ .s = #name }), \
        make(TYPE_Bool, (uc){ .b = readonly }) \
    }));
#define HIDDEN(...)
#define ATYPE(name, base) ZTYPE(name)
#define ZTYPE(name) types[TYPE_(name)] = (tType){ \
        make(TYPE_Type, (uc){ .p = &types[TYPE_Type] }), \
        vUndefined, \
        mklistalloc(list_new()) \
    };
#define TYPE(name, parent, flds) types[ct = TYPE_(name)] = (tType){ \
        make(TYPE_Type, (uc){ .p = &types[TYPE_Type] }), \
        make(TYPE_Type, (uc){ .p = &types[TYPE_(parent)] }), \
        mklistalloc(list_new()) \
    }; \
    flds
#define OP(key, name, argc, ...) ops[OP_(name)] = (tFunc){ \
        make(TYPE_Type, (uc){ .p = &types[TYPE_Func] }), \
        make(TYPE_Int, (uc){ .i = argc }), \
        vTrue, \
        &CAT(o, name) \
    };
#define FUNC(name, argc, ...) funcs[FUNC_(name)] = (tFunc){ \
        make(TYPE_Type, (uc){ .p = &types[TYPE_Func] }), \
        make(TYPE_Int, (uc){ .i = argc }), \
        vTrue, \
        &CAT(f, name) \
    };
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
    return (tList){ make(TYPE_Type, (uc){ .p = &types[TYPE_List] }), 0, 16, false, (unit*)malloc(16 * sizeof(unit)) };
}

void drop(tList* l, size_t n) {
    l->count -= n;
    if (l->capacity > 16 && l->count < l->capacity / 4) {
        l->capacity /= 2;
        l->__items = realloc(l->__items, l->capacity * sizeof(unit));
    }
}
void insert(tList* l, unit item, size_t index) {
    if (l->count == l->capacity) {
        l->capacity *= 2;
        l->__items = realloc(l->__items, l->capacity * sizeof(unit));
    }

    memmove(&l->__items[index + 1], &l->__items[index], (l->count - index) * sizeof(unit));
    l->__items[index] = item;
    l->count++;
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