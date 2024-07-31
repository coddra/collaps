#ifdef OPDEF

#define x (args[0])
#define y (args[1])
#define z (args[2])

#define gi getint
#define gf getfloat
#define gl getlen
#define gs getstr
#define ga getlist
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

#define OP(key, name, argc, ...) static inline unit CAT(FUNC_, OP_(name))(unit* args) __VA_ARGS__;

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
OP(".", PRINT, 1, {
    if (isi(x))
        printf("%ld\n", gi(x));
    else if (isf(x))
        printf("%f\n", gf(x));
    else if (iss(x))
        printf("%s\n", gs(x));
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
