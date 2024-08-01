#ifdef OPDEF

#define x (args[0])
#define y (args[1])
#define z (args[2])

#define gi getint
#define gf getfloat
#define gl getlist
#define gs getstr
#define go getop

#define isi(x) is(x, T_INT)
#define isf(x) is(x, T_FLOAT)
#define isn(x) (isi(x) || isf(x))
#define iss(x) is(x, T_STR)
#define isl(x) is(x, T_LIST)
#define isfu(x) is(x, T_FUNC)
#define isv(x) is(x, T_VOID)

#define asi(x) as(x, T_INT)
#define asf(x) as(x, T_FLOAT)

#define mi mkint
#define mf mkfloat
#define ms mkstr
#define ml mklist
#define mo mkop

#define OP(key, name, argc, ...) static inline unit CAT(OP_(name), _DEF)(unit* args) __VA_ARGS__;

#endif

#ifdef OP

// must be in alphabethic order, `./project test` confirms this
OP("!", NOT, 1, {
    if (isnull(x) ||
        (isi(x) && gi(x) == 0) ||
        (isf(x) && gf(x) == 0.0))
        return mi(1);
    else
        return mi(0);
})
OP("%", MOD, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mf(fmod(gf(asf(x)), gf(asf(y))));
        else 
            return mi(gi(x) % gi(y));
    }
    return mkvoid();
})
OP("&", AND, 2, {
    if (isi(x) && isi(y))
        return mi(gi(x) & gi(y));
    return mkvoid();
})
OP("&&", BAND, 2, {
    if (isnull(x) ||
        (isi(x) && gi(x) == 0) ||
        (isf(x) && gf(x) == 0.0))
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
    return mkvoid();
})
OP("**", POW, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mf(pow(gf(asf(x)), gf(asf(y))));
        else 
            return mi(pow(gi(x), gi(y)));
    }
    return mkvoid();
})
OP("+", ADD, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mf(gf(asf(x)) + gf(asf(y)));
        else 
            return mi(gi(x) + gi(y));
    }
    return mkvoid();
})
OP("-", SUB, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mf(gf(asf(x)) - gf(asf(y)));
        else 
            return mi(gi(x) - gi(y));
    }
    return mkvoid();
})
OP("/", DIV, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mf(gf(asf(x)) / gf(asf(y)));
        else 
            return mi(gi(x) / gi(y));
    }
    return mkvoid();
})
OP("<", LT, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mi(gf(asf(x)) < gf(asf(y)));
        else 
            return mi(gi(x) < gi(y));
    } else if (iss(x) && iss(y)) {
        return mi(strcmp(gs(x), gs(y)) < 0);
    }
    return mkvoid();
})
OP("<=", LE, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mi(gf(asf(x)) <= gf(asf(y)));
        else 
            return mi(gi(x) <= gi(y));
    } else if (iss(x) && iss(y)) {
        return mi(strcmp(gs(x), gs(y)) <= 0);
    }
    return mkvoid();
})
OP("==", EQ, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mi(gf(asf(x)) == gf(asf(y)));
        else 
            return mi(gi(x) == gi(y));
    } else if (iss(x) && iss(y)) {
        return mi(strcmp(gs(x), gs(y)) == 0);
    }
    return mkvoid();
})
OP(">", GT, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mi(gf(asf(x)) > gf(asf(y)));
        else 
            return mi(gi(x) > gi(y));
    } else if (iss(x) && iss(y)) {
        return mi(strcmp(gs(x), gs(y)) > 0);
    }
    return mkvoid();
})
OP(">=", GE, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            return mi(gf(asf(x)) >= gf(asf(y)));
        else 
            return mi(gi(x) >= gi(y));
    } else if (iss(x) && iss(y)) {
        return mi(strcmp(gs(x), gs(y)) >= 0);
    }
    return mkvoid();
})
OP("|", OR, 2, {
    if (isi(x) && isi(y))
        return mi(gi(x) | gi(y));
    return mkvoid();
})
OP("||", BOR, 2, {
    if (isnull(x) ||
        (isi(x) && gi(x) == 0) ||
        (isf(x) && gf(x) == 0.0))
        return y;
    else
        return x;
})

#endif

#ifdef OPDEF
#undef OP
#undef OPDEF
#endif

#ifdef FUNCDEF
#define FUNC(name, argc, ...) static inline unit CAT(FUNC_(name), _DEF)(unit* args) __VA_ARGS__
#endif

#ifdef FUNC
FUNC(print, 1, {
    const char* s = gs(invoke(funcs[FUNC_toString], x));
    printf("%s\n", s);
    if (!is(x, T_STR))
        free((void*)s);
    return mkvoid();
})
FUNC(toString, 1, {
    if (isi(x))
        return ms(itoa(gi(x)));
    else if (isf(x))
        return ms(ftoa(gf(x)));
    else if (iss(x))
        return x;
    else if (isl(x)) {
        char* res = (char*)malloc(2);
        res[0] = '['; res[1] = '\0';
        size_t len = 0;
        for (int i = 0; i < gl(x)->count; i++) {
            const char* item = gs(invoke(funcs[FUNC_toString], gl(x)->items[i]));
            len += strlen(item) + (i == 0 ? 0 : 3);
            res = (char*)realloc(res, len);
            if (i > 0)
                strcat(res, ", ");
            strcat(res, item);
            if (!is(gl(x)->items[i], T_STR))
                free((void*)item);
        }
        res = (char*)realloc(res, len + 2);
        strcat(res, "]");
        return ms(res);
    }
    else
        return mkvoid();
})
#endif

#ifdef FUNCDEF
#undef FUNC
#undef FUNCDEF
#endif