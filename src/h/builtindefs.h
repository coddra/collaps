#ifdef ABBREVS

#define x (args[0])
#define y (args[1])
#define z (args[2])

#define gi getint
#define gb getbool
#define gf getfloat
#define gl getlist
#define gs getstr
#define go getop

#define isi(x) is(x, TYPE_Int)
#define isb(x) is(x, TYPE_Bool)
#define isf(x) is(x, TYPE_Float)
#define isn(x) (isi(x) || isf(x))
#define iss(x) is(x, TYPE_String)
#define isl(x) is(x, TYPE_List)
#define isfu(x) is(x, TYPE_Func)
#define isv(x) is(x, TYPE_Void)

#define asi(x) as(x, TYPE_Int)
#define asf(x) as(x, TYPE_Float)

#define mi(x) mkint(x)
#define mb(x) make(TYPE_Bool, x)
#define mf(x) mkfloat(x)
#define ms(x) make(TYPE_String, x)
#define ml(x) make(TYPE_List, x)
#define mv() make(TYPE_Void)

#endif // ABBREVS

#ifdef TYPE
#define FIELD(name) FLD(name, false)
#define RFIELD(name) FLD(name, true)
ZTYPE(Void)
TYPE(Bool, HIDDEN(uint64_t v))
TYPE(Field, RFIELD(name) RFIELD(readonly))
TYPE(Float, HIDDEN(double v))
TYPE(Func, RFIELD(name) RFIELD(argc) RFIELD(builtin) HIDDEN(unit (*__invoke)(unit*)))
TYPE(Int, HIDDEN(int64_t v))
TYPE(List, RFIELD(count) RFIELD(capacity) RFIELD(readonly) HIDDEN(unit* __items))
TYPE(String, HIDDEN(const char* v))
TYPE(Type, RFIELD(name) RFIELD(fields))
#undef FIELD
#undef RFIELD
#endif // TYPE

#ifdef OP
// must be in alphabethic order, `./project test` confirms this
OP("!", NOT, 1, {
    if (isnull(x) ||
        (isi(x) && gi(x) == 0) ||
        (isf(x) && gf(x) == 0.0))
        return mb(true);
    else
        return mb(false);
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

#ifdef FUNC
// must be in alphabethic order, `./project test` confirms this
FUNC(print, 1, {
    const char* s = gs(invoke(funcs[FUNC_toString], x));
    printf("%s\n", s);
    if (!iss(x) && !isb(x))
        free((void*)s);
    return mv();
})
FUNC(toString, 1, {
    if (isi(x))
        return ms(itoa(gi(x)));
    else if (isf(x))
        return ms(ftoa(gf(x)));
    else if (iss(x))
        return x;
    else if (isb(x))
        return ms(gb(x) ? "true" : "false");
    else if (isl(x)) {
        char* res = (char*)malloc(2);
        res[0] = '['; res[1] = '\0';
        size_t len = 0;
        for (int i = 0; i < gl(x)->count; i++) {
            const char* item = gs(invoke(funcs[FUNC_toString], gl(x)->__items[i]));
            len += strlen(item) + (i == 0 ? 0 : 3);
            res = (char*)realloc(res, len);
            if (i > 0)
                strcat(res, ", ");
            strcat(res, item);
            if (!iss(gl(x)->__items[i]))
                free((void*)item);
        }
        res = (char*)realloc(res, len + 2);
        strcat(res, "]");
        return ms(res);
    } else {
        size_t len = 3;
        char* res = (char*)malloc(len);
        res[0] = '{'; res[1] = ' '; res[2] = '\0';
        tType t = types[gettype(x)];
        tList* fields = gl(t.fields);
        for (int i = 0; i < fields->count; i++) {
            const char* field = gs(getfield(fields->__items[i])->name);
            unit u = *((unit*)getptr(x) + i);
            const char* value = gs(invoke(funcs[FUNC_toString], u));
            len += strlen(field) + strlen(value) + 4;
            res = (char*)realloc(res, len);
            strcat(res, field);
            strcat(res, ": ");
            strcat(res, value);
            if (i < fields->count - 1)
                strcat(res, ", ");

            if (!iss(u) && !isb(u))
                free((void*)value);
        }
        strcat(res, " }");
        return ms(res);
    }
})
FUNC(typeof, 1, {
    return make(TYPE_Type, &types[gettype(x)]);
})
#endif // FUNC
