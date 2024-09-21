#include "h/context.h"


tList create_environment(const char *name) {
    tList fields = list_new();
    push(&fields, make(TYPE_Field, mkfieldalloc((tField){ make(TYPE_Type, &types[TYPE_Field]), make(TYPE_String, "type"), make(TYPE_Bool, true) })));

    tType* type = (tType*)malloc(sizeof(tType));
    *type = (tType){
        .type = make(TYPE_Type, &types[TYPE_Type]),
        .name = make(TYPE_String, "Global"),
        .parent = make(TYPE_Type, &types[TYPE_Object]),
        .fields = mklistalloc(fields),    
    };

    tList environment = list_new();
    push(&environment, make(TYPE_Type, type));

    return environment;
}