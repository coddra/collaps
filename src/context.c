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
        int cmp = strncmp(get_str(get(tField*, fields->__items[m])->name), start, length);
        if (cmp == 0)
            cmp = length < strlen(get_str(get(tField*, fields->__items[m])->name));
        if (cmp < 0) l = m + 1;
        else if (cmp > 0) r = m;
        else return m;
    }
    return -m - 1;
}

unit resolve_symbol(context* ctx, const char* start, size_t length) {
    int m;
    do {
        tList* fields = get(tList*, get(tType*, ctx->environment.__items[0])->fields);
        m = binsearch(fields, start, length);
        if (m < 0) ctx = ctx->parent;
    } while (m < 0 && ctx != NULL);
    return m < 0 ? vUndefined : ctx->environment.__items[m + 1];
}
