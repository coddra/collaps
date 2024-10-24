#ifdef ABBREVS

#define x (args[0])
#define y (args[1])
#define z (args[2])

#define gi get_int
#define gb get_bool
#define gf get_float
#define gl(x) get(tList*, x)
#define gs(x) get(tString*, x)

#define isi(x) is(x, TYPE_Int)
#define isb(x) is(x, TYPE_Bool)
#define isf(x) is(x, TYPE_Float)
#define isn(x) (isi(x) || isf(x))
#define iss(x) is(x, TYPE_String)
#define isl(x) is(x, TYPE_List)
#define isfu(x) is(x, TYPE_Func)
#define isu(x) is(x, TYPE_Undefined)

#define asi(x) as(x, TYPE_Int)
#define asf(x) as(x, TYPE_Float)

#define mi(x) make(TYPE_Int, (uc){ .i = x })
#define mb(x) make(TYPE_Bool, (uc){ .b = x })
#define mf(x) make(TYPE_Float, (uc){ .d = x })
#define ms(x) make(TYPE_String, (uc){ .s = x })
#define ml(x) make(TYPE_List, (uc){ .p = x })
#define vu vUndefined

#endif // ABBREVS

#ifdef TYPE
#define FIELD(name) FLD(name, false)
#define RFIELD(name) FLD(name, true)
ATYPE(Bool, uint64_t)
TYPE(Block, Object, RFIELD(code) RFIELD(location))
TYPE(Field, Object, RFIELD(name) RFIELD(readonly))
ATYPE(Float, double)
TYPE(Func, Object, RFIELD(argc) RFIELD(builtin) HIDDEN(unit (*invoke)(context*, unit*)))
ATYPE(Int, int64_t)
TYPE(List, Object, RFIELD(count) RFIELD(capacity) RFIELD(readonly) HIDDEN(unit* __items))
TYPE(Location, Object, RFIELD(file) RFIELD(line) RFIELD(column))
ATYPE(Object, void*)
TYPE(String, Object, RFIELD(length) HIDDEN(const char* s))
ATYPE(Symbol, const char*)
TYPE(Type, Object, RFIELD(parent) RFIELD(fields))
ZTYPE(Undefined)
#undef FIELD
#undef RFIELD
#endif // TYPE

// must be in alphabethic order, `./project test` confirms this
#ifdef OP
OP("!", NOT, 1, {
    return mb(!gb(tFunc_invoke(ctx, funcs[FUNC_bool], x)));
})
OP("%", MOD, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mf(fmod(gf(asf(x)), gf(asf(y))));
        else
            return mi(gi(x) % gi(y));
    }
    return vu;
})
OP("&", AND, 2, {
    if (isi(x) && isi(y))
        return mi(gi(x) & gi(y));
    return vu;
})
OP("&&", BAND, 2, {
    if (is_null(x) ||
        (isi(x) && gi(x) == 0) ||
        (isf(x) && gf(x) == 0.0) ||
        (isb(x) && !gb(x)))
        return x;
    else
        return y;
})
OP("*", MUL, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mf(gf(asf(x)) * gf(asf(y)));
        else
            return mi(gi(x) * gi(y));
    }
    return vu;
})
OP("**", POW, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mf(pow(gf(asf(x)), gf(asf(y))));
        else
            return mi(pow(gi(x), gi(y)));
    }
    return vu;
})
OP("+", ADD, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mf(gf(asf(x)) + gf(asf(y)));
        else
            return mi(gi(x) + gi(y));
    }
    return vu;
})
OP("-", SUB, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mf(gf(asf(x)) - gf(asf(y)));
        else
            return mi(gi(x) - gi(y));
    }
    return vu;
})
OP("/", DIV, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mf(gf(asf(x)) / gf(asf(y)));
        else
            return mi(gi(x) / gi(y));
    }
    return vu;
})
OP(":", DECLARE, 2, {
    if (is(x, TYPE_Symbol))
        declare_symbol(ctx, get(char*, x), y);
    return vu;
})
OP("<", LT, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mb(gf(asf(x)) < gf(asf(y)));
        else
            return mb(gi(x) < gi(y));
    } else if (iss(x) && iss(y)) {
        return mb(tString_compare(gs(x), gs(y)) < 0);
    }
    return vu;
})
OP("<=", LE, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mb(gf(asf(x)) <= gf(asf(y)));
        else
            return mb(gi(x) <= gi(y));
    } else if (iss(x) && iss(y)) {
        return mb(tString_compare(gs(x), gs(y)) <= 0);
    }
    return vu;
})
OP("==", EQ, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mb(gf(asf(x)) == gf(asf(y)));
        else
            return mb(gi(x) == gi(y));
    } else if (iss(x) && iss(y)) {
        return mb(tString_compare(gs(x), gs(y)) == 0);
    }
    return vu;
})
OP(">", GT, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mb(gf(asf(x)) > gf(asf(y)));
        else
            return mb(gi(x) > gi(y));
    } else if (iss(x) && iss(y)) {
        return mb(tString_compare(gs(x), gs(y)) > 0);
    }
    return vu;
})
OP(">=", GE, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mb(gf(asf(x)) >= gf(asf(y)));
        else
            return mb(gi(x) >= gi(y));
    } else if (iss(x) && iss(y)) {
        return mb(tString_compare(gs(x), gs(y)) >= 0);
    }
    return vu;
})
#endif // OP
// More OPs below

#ifndef FREENONCONST
#define FREENONCONST(u, str) if (!(iss(u) || isb(u) || isu(u))) free((void*)str)
#endif // FREENONCONST
#ifdef FUNC
FUNC(bool, 1, {
    return mb(
        !isu(x) &&
        (!isb(x) || gb(x)) &&
        (!isi(x) || gi(x) != 0) &&
        (!isf(x) || gf(x) != 0.0) &&
        (!iss(x) || gs(x)->length != 0) &&
        (!isl(x) || gl(x)->count != 0) &&
        !is_null(x));
})
FUNC(print, 1, {
    tString* s = gs(tFunc_invoke(ctx, funcs[FUNC_toString], x));
    printf("%s\n", s->s);
    FREENONCONST(x, s->s);
    return vu;
})
FUNC(toString, 1, {
    char* res;
    size_t len;
    switch (get_typeid(x)) {
        case TYPE_Undefined: return ms("undefined");
        case TYPE_Bool: return ms(gb(x) ? "true" : "false");
        case TYPE_Int: return ms(itoa(gi(x)));
        case TYPE_Float: return ms(ftoa(gf(x)));
        case TYPE_String: return x;
        case TYPE_Symbol:
            len = strlen(get(char*, x)) + 2;
            res = (char*)malloc(len);
            res[0] = '`'; res[1] = '\0';
            strcat(res, get(char*, x));
            res[len - 1] = '`';
            return ms(res);
        case TYPE_List:
            len = 2;
            res = (char*)malloc(len);
            res[0] = '['; res[1] = '\0';
            for (int i = 0; i < gl(x)->count; i++) {
                tString* item = gs(tFunc_invoke(ctx, funcs[FUNC_toString], gl(x)->__items[i]));
                len += item->length + (i == 0 ? 0 : 3);
                res = (char*)realloc(res, len);
                if (i > 0)
                    strcat(res, ", ");
                strcat(res, item->s);
                FREENONCONST(gl(x)->__items[i], item);
            }
            res = (char*)realloc(res, len + 2);
            strcat(res, "]");
            return ms(res);
        default:
            len = 3;
            res = (char*)malloc(len);
            res[0] = '{'; res[1] = ' '; res[2] = '\0';
            tType t = types[get_typeid(x)];
            tList* fields = gl(t.fields);
            for (int i = 0; i < fields->count; i++) {
                tString* field = gs(get(tField*, fields->__items[i])->name);
                unit u = get(unit*, x)[i + 1];
                tString* value = gs(tFunc_invoke(ctx, funcs[FUNC_toString], u));
                len += field->length + value->length + 4;
                res = (char*)realloc(res, len);
                strcat(res, field->s);
                strcat(res, ": ");
                strcat(res, value->s);
                if (i < fields->count - 1)
                    strcat(res, ", ");
                FREENONCONST(u, value->s);
            }
            strcat(res, " }");
            return ms(res);
    }
})
FUNC(typeof, 1, {
    if (get_typeid(x) == TYPE_Object)
        return make(TYPE_Type, (uc){ .p = get(tType*, *get(unit*, x)) });
    return make(TYPE_Type, (uc){ .p = &types[get_typeid(x)] });
})
#endif // FUNC

#ifdef OP
OP("|", OR, 2, {
    if (isi(x) && isi(y))
        return mi(gi(x) | gi(y));
    return vu;
})
OP("||", BOR, 2, {
    if (is_null(x) ||
        (isi(x) && gi(x) == 0) ||
        (isf(x) && gf(x) == 0.0) ||
        (isb(x) && !gb(x)))
        return y;
    else
        return x;
})
#endif // OP