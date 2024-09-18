#ifdef ABBREVS

#define x (args[0])
#define y (args[1])
#define z (args[2])

#define gi getint
#define gb getbool
#define gf getfloat
#define gl(x) get(tList*, x)
#define gs getstr

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

#define mi(x) mkint(x)
#define mb(x) make(TYPE_Bool, x)
#define mf(x) mkfloat(x)
#define ms(x) make(TYPE_String, x)
#define ml(x) make(TYPE_List, x)
#define mv() make(TYPE_Undefined)

#endif // ABBREVS

#ifdef TYPE
#define FIELD(name) FLD(name, false)
#define RFIELD(name) FLD(name, true)
TYPE(Bool, Object, HIDDEN(uint64_t v))
TYPE(Field, Object, RFIELD(name) RFIELD(readonly))
TYPE(Float, Object, HIDDEN(double v))
TYPE(Func, Object, RFIELD(name) RFIELD(argc) RFIELD(builtin) HIDDEN(unit (*__invoke)(unit*)))
TYPE(Int, Object, HIDDEN(int64_t v))
TYPE(List, Object, RFIELD(count) RFIELD(capacity) RFIELD(readonly) HIDDEN(unit* __items))
ZTYPE(Object)
TYPE(String, Object, HIDDEN(const char* v))
TYPE(Type, Object, RFIELD(name) RFIELD(parent) RFIELD(fields))
ZTYPE(Undefined)
#undef FIELD
#undef RFIELD
#endif // TYPE

#ifdef OP
// must be in alphabethic order, `./project test` confirms this
OP("!", NOT, 1, {
    return mb(!gb(invoke(funcs[FUNC_bool], x)));
})
OP("%", MOD, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mf(fmod(gf(asf(x)), gf(asf(y))));
        else 
            return mi(gi(x) % gi(y));
    }
    return mv();
})
OP("&", AND, 2, {
    if (isi(x) && isi(y))
        return mi(gi(x) & gi(y));
    return mv();
})
OP("&&", BAND, 2, {
    if (isnull(x) ||
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
    return mv();
})
OP("**", POW, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mf(pow(gf(asf(x)), gf(asf(y))));
        else 
            return mi(pow(gi(x), gi(y)));
    }
    return mv();
})
OP("+", ADD, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mf(gf(asf(x)) + gf(asf(y)));
        else 
            return mi(gi(x) + gi(y));
    }
    return mv();
})
OP("-", SUB, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mf(gf(asf(x)) - gf(asf(y)));
        else 
            return mi(gi(x) - gi(y));
    }
    return mv();
})
OP("/", DIV, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mf(gf(asf(x)) / gf(asf(y)));
        else 
            return mi(gi(x) / gi(y));
    }
    return mv();
})
OP("<", LT, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mb(gf(asf(x)) < gf(asf(y)));
        else 
            return mb(gi(x) < gi(y));
    } else if (iss(x) && iss(y)) {
        return mb(strcmp(gs(x), gs(y)) < 0);
    }
    return mv();
})
OP("<=", LE, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mb(gf(asf(x)) <= gf(asf(y)));
        else 
            return mb(gi(x) <= gi(y));
    } else if (iss(x) && iss(y)) {
        return mb(strcmp(gs(x), gs(y)) <= 0);
    }
    return mv();
})
OP("==", EQ, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mb(gf(asf(x)) == gf(asf(y)));
        else 
            return mb(gi(x) == gi(y));
    } else if (iss(x) && iss(y)) {
        return mb(strcmp(gs(x), gs(y)) == 0);
    }
    return mv();
})
OP(">", GT, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mb(gf(asf(x)) > gf(asf(y)));
        else 
            return mb(gi(x) > gi(y));
    } else if (iss(x) && iss(y)) {
        return mb(strcmp(gs(x), gs(y)) > 0);
    }
    return mv();
})
OP(">=", GE, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mb(gf(asf(x)) >= gf(asf(y)));
        else 
            return mb(gi(x) >= gi(y));
    } else if (iss(x) && iss(y)) {
        return mb(strcmp(gs(x), gs(y)) >= 0);
    }
    return mv();
})
OP("|", OR, 2, {
    if (isi(x) && isi(y))
        return mi(gi(x) | gi(y));
    return mv();
})
OP("||", BOR, 2, {
    if (isnull(x) ||
        (isi(x) && gi(x) == 0) ||
        (isf(x) && gf(x) == 0.0) ||
        (isb(x) && !gb(x)))
        return y;
    else
        return x;
})
#endif // OP

#ifndef FREENONCONST
#define FREENONCONST(u, str) if (!iss(u) && !isb(u) && !isu(u)) free((void*)str)
#endif // FREENONCONST
#ifdef FUNC
// must be in alphabethic order, `./project test` confirms this
FUNC(bool, 1, {
    return mb(
        !isu(x) &&
        (!isb(x) || gb(x)) &&
        (!isi(x) || gi(x) != 0) &&
        (!isf(x) || gf(x) != 0.0) &&
        (!iss(x) || strlen(gs(x)) != 0) &&
        (!isl(x) || gl(x)->count != 0) &&
        !isnull(x));
})
FUNC(print, 1, {
    const char* s = gs(invoke(funcs[FUNC_toString], x));
    printf("%s\n", s);
    FREENONCONST(x, s);
    return mv();
})
FUNC(toString, 1, {
    char* res;
    size_t len;
    switch (gettypeid(x)) {
        case TYPE_Undefined: return ms("undefined");
        case TYPE_Bool: return ms(gb(x) ? "true" : "false");
        case TYPE_Int: return ms(itoa(gi(x)));
        case TYPE_Float: return ms(ftoa(gf(x)));
        case TYPE_String: return x;
        case TYPE_List:
            len = 2;
            res = (char*)malloc(len);
            res[0] = '['; res[1] = '\0';
            for (int i = 0; i < gl(x)->count; i++) {
                const char* item = gs(invoke(funcs[FUNC_toString], gl(x)->__items[i]));
                len += strlen(item) + (i == 0 ? 0 : 3);
                res = (char*)realloc(res, len);
                if (i > 0)
                    strcat(res, ", ");
                strcat(res, item);
                FREENONCONST(gl(x)->__items[i], item);
            }
            res = (char*)realloc(res, len + 2);
            strcat(res, "]");
            return ms(res);
        default:
            len = 3;
            res = (char*)malloc(len);
            res[0] = '{'; res[1] = ' '; res[2] = '\0';
            tType t = types[gettypeid(x)];
            tList* fields = gl(t.fields);
            for (int i = 0; i < fields->count; i++) {
                const char* field = gs(get(tField*, fields->__items[i])->name);
                unit u = *(get(unit*, x) + i);
                const char* value = gs(invoke(funcs[FUNC_toString], u));
                len += strlen(field) + strlen(value) + 4;
                res = (char*)realloc(res, len);
                strcat(res, field);
                strcat(res, ": ");
                strcat(res, value);
                if (i < fields->count - 1)
                    strcat(res, ", ");
                FREENONCONST(u, value);
            }
            strcat(res, " }");
            return ms(res);
    }
})
FUNC(typeof, 1, {
    return make(TYPE_Type, &types[gettypeid(x)]);
})
#endif // FUNC
