#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "h/builtins.h"
#include "h/context.h"


#define INT_WIDTH 63ul // max 4 611 686 018 427 387 903, min -4 611 686 018 427 387 904
#define FLOAT_WIDTH 62ul // smallest positive value: 8.9e-307
#define TYPE_WIDTH 14ul // up to 16384 different types
#define INT_MASK ((1ul << INT_WIDTH) - 1)
#define FLOAT_MASK ((1ul << FLOAT_WIDTH) - 1)
#define TYPE_MASK ((1ul << TYPE_WIDTH) - 1)
#define SIGN_MASK (1ul << (INT_WIDTH - 1))

#define OBJ_T (3ul << (TYPE_WIDTH + PTR_WIDTH))
#define FLOAT_T (2ul << (TYPE_WIDTH + PTR_WIDTH))

unit make(enum TYPE type, uc u) {
    tString* s;
    if (type == TYPE_Undefined)
        return ((unit)TYPE_Undefined << PTR_WIDTH) | OBJ_T;
    switch (type) {
        case TYPE_Bool: return u.b | ((unit)type << PTR_WIDTH) | OBJ_T;
        case TYPE_Int: return u.i & INT_MASK;
        case TYPE_Float: return (u.u >> (64 - FLOAT_WIDTH)) | FLOAT_T;
        case TYPE_String:
            s = malloc(sizeof(tString));
            *s = (tString){ make(TYPE_Type, (uc){ .p = &types[TYPE_String] }), strlen(u.s), u.s };
            u.p = s;
        default:
            return ((unit)u.p & PTR_MASK) | ((unit)type << PTR_WIDTH) | OBJ_T;
    }
}
unit make_alloc(enum TYPE type, void* p) {
    void* dest = malloc(size_of(type));
    memmove(dest, p, size_of(type));
    return make(type, (uc){ .p = dest });
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

size_t size_of(enum TYPE type) {
    return (get(tList*, types[type].fields)->count + 1) * sizeof(unit);
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
        mklistalloc(tList_new()) \
    };
#define TYPE(name, parent, flds) types[ct = TYPE_(name)] = (tType){ \
        make(TYPE_Type, (uc){ .p = &types[TYPE_Type] }), \
        make(TYPE_Type, (uc){ .p = &types[TYPE_(parent)] }), \
        mklistalloc(tList_new()) \
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
tList tList_new() {
    return (tList){ make(TYPE_Type, (uc){ .p = &types[TYPE_List] }), 0, 16, false, (unit*)malloc(16 * sizeof(unit)) };
}

void tList_drop(tList* l, size_t n) {
    l->count -= n;
    if (l->capacity > 16 && l->count < l->capacity / 4) {
        l->capacity /= 2;
        l->__items = realloc(l->__items, l->capacity * sizeof(unit));
    }
}
void tList_insert(tList* l, unit item, size_t index) {
    if (index > l->count) {
        // ERROR: can't insert item
        return;
    }

    if (l->count == l->capacity) {
        l->capacity *= 2;
        l->__items = realloc(l->__items, l->capacity * sizeof(unit));
    }

    memmove(&l->__items[index + 1], &l->__items[index], (l->count - index) * sizeof(unit));
    l->__items[index] = item;
    l->count++;
}

unit tFunc_invoke(context* ctx, tFunc f, ...) {
    unit args[MAX_ARGC] = {0};
    va_list va;
    va_start(va, f);
    for (int i = 0; i < f.argc; i++)
        args[i] = va_arg(va, unit);
    va_end(va);
    return f.invoke(ctx, args);
}

int tString_compare(tString* l, tString* r) {
    for (int i = 0; i < l->length && i < r->length; i++) {
        if (l->s[i] < r->s[i])
            return -1;
        else if (l->s[i] > r->s[i])
            return 1;
    }
    return l->length < r->length ? -1 : l->length > r->length ? 1 : 0;
}