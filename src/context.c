#include "h/context.h"


tList create_environment() {
    tList fields = list_new();
    push(&fields, make(TYPE_Field, mkfieldalloc((tField){ make(TYPE_Type, &types[TYPE_Field]), make(TYPE_String, "__type"), make(TYPE_Bool, true) })));

    tType* type = (tType*)malloc(sizeof(tType));
    *type = (tType){
        .__type = make(TYPE_Type, &types[TYPE_Type]),
        .parent = make(TYPE_Type, &types[TYPE_Object]),
        .fields = mklistalloc(fields),    
    };

    tList environment = list_new();
    push(&environment, make(TYPE_Type, type));

    return environment;
}

void load_builtins(context *ctx) {
#define FUNC(name, ...) \
    push(get(tList*, get(tType*, ctx->environment.__items[0])->fields), mkfieldalloc((tField){ make(TYPE_Type, &types[TYPE_Field]), make(TYPE_String, #name), make(TYPE_Bool, true) })); \
    push(&ctx->environment, make(TYPE_Func, &funcs[FUNC_(name)]));
#define OP(key, name, ...) \
    push(get(tList*, get(tType*, ctx->environment.__items[0])->fields), mkfieldalloc((tField){ make(TYPE_Type, &types[TYPE_Field]), make(TYPE_String, key), make(TYPE_Bool, true) })); \
    push(&ctx->environment, make(TYPE_Func, &ops[OP_(name)]));
#   include "h/builtindefs.h"
#undef FUNC
#undef OP
}