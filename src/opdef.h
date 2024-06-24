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

#define isi(x) is(x, M_INT)
#define isf(x) is(x, M_FLOAT)
#define isn(x) is(x, M_NUM)
#define iss(x) is(x, M_STR)
#define isl(x) is(x, M_LIST)
#define isa(x) is(x, M_ARR)
#define iso(x) is(x, M_OP)
#define isv(x) is(x, M_VOID)

#define asi(x) as(x, M_INT)
#define asf(x) as(x, M_FLOAT)
#define ass(x) as(x, M_STR)
#define asl(x) as(x, M_LIST)
#define aso(x) as(x, M_OP)

#define mi mkint
#define mf mkfloat
#define ms mkstr
#define ml mklist
#define mo mkop

#endif

#ifdef OP

OP('+', ADD, 2, M_NUM, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mf(gf(asf(x)) + gf(asf(y))) );
        else 
            ret( mi(gi(x) + gi(y)) );
    }
})
OP('-', SUB, 2, M_NUM, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mf(gf(asf(x)) - gf(asf(y))) );
        else 
            ret( mi(gi(x) - gi(y)) );
    }
})
OP('*', MUL, 2, M_NUM, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mf(gf(asf(x)) * gf(asf(y))) );
        else 
            ret( mi(gi(x) * gi(y)) );
    }
})
OP('/', DIV, 2, M_NUM, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mf(gf(asf(x)) / gf(asf(y))) );
        else 
            ret( mi(gi(x) / gi(y)) );
    }
})
OP('%', MOD, 2, M_NUM, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mf(fmod(gf(asf(x)), gf(asf(y)))) );
        else 
            ret( mi(gi(x) % gi(y)) );
    }
})

OP('=', EQ, 2, M_INT, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mi(gf(asf(x)) == gf(asf(y))) );
        else 
            ret( mi(gi(x) == gi(y)) );
    }
})
OP('<', LT, 2, M_INT, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mi(gf(asf(x)) < gf(asf(y))) );
        else 
            ret( mi(gi(x) < gi(y)) );
    }
})
OP('>', GT, 2, M_INT, {
    if (isn(x) && isn(y)) {
        if (isf(x) || isf(y))
            ret( mi(gf(asf(x)) > gf(asf(y))) );
        else 
            ret( mi(gi(x) > gi(y)) );
    }
})

OP('~', NOT, 1, M_INT, {
    if (isv(x) ||
        (isi(x) && gi(x) == 0) ||
        (isf(x) && gf(x) == 0.0) ||
        (isa(x) && gl(x) == 0))
        ret( mi(1) );
    else
        ret( mi(0) );
})

OP('&', AND, 2, M_ANY, {
    if (isv(x) ||
        (isi(x) && gi(x) == 0) ||
        (isf(x) && gf(x) == 0.0) ||
        (isa(x) && gl(x) == 0))
        ret( x );
    else
        ret( y );
})
OP('|', OR, 2, M_ANY, {
    if (isv(x) ||
        (isi(x) && gi(x) == 0) ||
        (isf(x) && gf(x) == 0.0) ||
        (isa(x) && gl(x) == 0))
        ret( y );
    else
        ret( x );
})

OP('.', PRINT, 1, M_VOID, {
    if (isi(x))
        printf("%ld\n", gi(x));
    else if (isf(x))
        printf("%f\n", gf(x));
    else if (iss(x))
        printf("%s\n", gs(x));
})

#endif
