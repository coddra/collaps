#include "h/context.h"
#include "h/builtins.h"


tList create_environment() {
    tType* type = (tType*)malloc(sizeof(tType));
    *type = (tType){
        .__type = make(TYPE_Type, (uc){ .p = &types[TYPE_Type] }),
        .parent = make(TYPE_Type, (uc){ .p = &types[TYPE_Object] }),
        .fields = mklistalloc(list_new()),
    };

    tList environment = list_new();
    push(&environment, make(TYPE_Type, (uc){ .p = type }));

    return environment;
}

void load_builtins(context *ctx) {
#define FUNC(name, ...) \
    push(get(tList*, get(tType*, ctx->environment.__items[0])->fields), mkfieldalloc((tField){ \
        make(TYPE_Type, (uc){ .p = &types[TYPE_Field] }), \
        make(TYPE_String, (uc){ .s = #name }), \
        vTrue \
    })); \
    push(&ctx->environment, make(TYPE_Func, (uc){ .p = &funcs[FUNC_(name)] }));
#define OP(key, name, ...) \
    push(get(tList*, get(tType*, ctx->environment.__items[0])->fields), mkfieldalloc((tField){ \
        make(TYPE_Type, (uc){ .p = &types[TYPE_Field] }), \
        make(TYPE_String, (uc){ .s = key }), \
        vTrue \
    })); \
    push(&ctx->environment, make(TYPE_Func, (uc){ .p = &ops[OP_(name)] }));
#   include "h/builtindefs.h"
#undef FUNC
#undef OP
}

int binsearch(tList* fields, const char* start, size_t length) {
    size_t l = 0, r = fields->count;
    int m = 1;
    while (l < r) {
        m = (l + r) / 2;
        int cmp = strncmp(get(char*, get(tField*, fields->__items[m])->name), start, length);
        if (cmp == 0)
            cmp = length < strlen(get(char*, get(tField*, fields->__items[m])->name));
        if (cmp < 0) l = m + 1;
        else if (cmp > 0) r = m;
        else return m;
    }
    return -m - 1;
}
int search(tList* fields, const char* start, size_t length) {
    for (int i = 0; i < fields->count; i++) {
        const char* name = get(char*, get(tField*, fields->__items[i])->name);
        if (strncmp(name, start, length) == 0 && strlen(name) == length)
            return i;
    }
    return -fields->count - 1;
}

bool is_global(context* ctx) {
    return ctx->parent == NULL;
}

unit resolve_symbol(context* ctx, const char* start, size_t length) {
    int i;
    do {
        tList* fields = get(tList*, get(tType*, ctx->environment.__items[0])->fields);
        i = is_global(ctx) ? binsearch(fields, start, length) : search(fields, start, length);
        if (i < 0) ctx = ctx->parent;
    } while (i < 0 && ctx != NULL);
    return i < 0 ? vUndefined : ctx->environment.__items[i + 1];
}
void declare_symbol(context* ctx, const char* symbol, unit value) {
    size_t len = strlen(symbol);
    tList* fields = get(tList*, get(tType*, ctx->environment.__items[0])->fields);
    int i = is_global(ctx) ? binsearch(fields, symbol, len) : search(fields, symbol, len);

    if (i >= 0) {
        // ERROR: symbol already declared
        return;
    }
    i = -i + 1;
    insert(fields, mkfieldalloc((tField){
        .__type = make(TYPE_Type, (uc){ .p = &types[TYPE_Field] }),
        .name = make(TYPE_String, (uc){ .s = symbol }),
        .readonly = vFalse,
    }), i);
    insert(&ctx->environment, value, i + 1);
}