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

#endif

#ifdef OP

// must be in alphabethic order, `./project test` confirms this
OP("!", NOT, 1, {
    if (isnull(x) ||
        (isi(x) && gi(x) == 0) ||
        (isf(x) && gf(x) == 0.0))
        ret( mi(1) );
    else
        ret( mi(0) );
})
OP("%", MOD, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mf(fmod(gf(asf(x)), gf(asf(y)))) );
        else 
            ret( mi(gi(x) % gi(y)) );
    }
})
OP("&", AND, 2, {
    if (isi(x) && isi(y))
        ret( mi(gi(x) & gi(y)) );
})
OP("&&", BAND, 2, {
    if (isnull(x) ||
        (isi(x) && gi(x) == 0) ||
        (isf(x) && gf(x) == 0.0))
        ret( x );
    else
        ret( y );
})
OP("*", MUL, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mf(gf(asf(x)) * gf(asf(y))) );
        else 
            ret( mi(gi(x) * gi(y)) );
    }
})
OP("+", ADD, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mf(gf(asf(x)) + gf(asf(y))) );
        else 
            ret( mi(gi(x) + gi(y)) );
    }
})
OP("-", SUB, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mf(gf(asf(x)) - gf(asf(y))) );
        else 
            ret( mi(gi(x) - gi(y)) );
    }
})
OP(".", PRINT, 1, {
    if (isi(x))
        printf("%ld\n", gi(x));
    else if (isf(x))
        printf("%f\n", gf(x));
    else if (iss(x))
        printf("%s\n", gs(x));
})
OP("/", DIV, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mf(gf(asf(x)) / gf(asf(y))) );
        else 
            ret( mi(gi(x) / gi(y)) );
    }
})
OP("<", LT, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mi(gf(asf(x)) < gf(asf(y))) );
        else 
            ret( mi(gi(x) < gi(y)) );
    } else if (iss(x) && iss(y)) {
        ret( mi(strcmp(gs(x), gs(y)) < 0) );
    }
})
OP("<=", LE, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mi(gf(asf(x)) <= gf(asf(y))) );
        else 
            ret( mi(gi(x) <= gi(y)) );
    } else if (iss(x) && iss(y)) {
        ret( mi(strcmp(gs(x), gs(y)) <= 0) );
    }
})
OP("==", EQ, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mi(gf(asf(x)) == gf(asf(y))) );
        else 
            ret( mi(gi(x) == gi(y)) );
    } else if (iss(x) && iss(y)) {
        ret( mi(strcmp(gs(x), gs(y)) == 0) );
    }
})
OP(">", GT, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mi(gf(asf(x)) > gf(asf(y))) );
        else 
            ret( mi(gi(x) > gi(y)) );
    } else if (iss(x) && iss(y)) {
        ret( mi(strcmp(gs(x), gs(y)) > 0) );
    }
})
OP(">=", GE, 2, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mi(gf(asf(x)) >= gf(asf(y))) );
        else 
            ret( mi(gi(x) >= gi(y)) );
    } else if (iss(x) && iss(y)) {
        ret( mi(strcmp(gs(x), gs(y)) >= 0) );
    }
})
OP("|", OR, 2, {
    if (isi(x) && isi(y))
        ret( mi(gi(x) | gi(y)) );
})
OP("||", BOR, 2, {
    if (isnull(x) ||
        (isi(x) && gi(x) == 0) ||
        (isf(x) && gf(x) == 0.0))
        ret( y );
    else
        ret( x );
})

#endif
