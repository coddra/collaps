#ifdef OP

OP('+', ADD, 2, M_NUM, (unit x, unit y) {
    if (is(x, M_NUM) && is(y, M_NUM)) {
        if (is(x, M_FLOAT) || is(y, M_FLOAT))
            return mkfloat(getfloat(as(x, M_FLOAT)) + getfloat(as(y, M_FLOAT)));
        else 
            return mkint(getint(x) + getint(y));
    }
    return mkvoid();
})
OP('-', SUB, 2, M_NUM, (unit x, unit y) {
    if (is(x, M_NUM) && is(y, M_NUM)) {
        if (is(x, M_FLOAT) || is(y, M_FLOAT))
            return mkfloat(getfloat(as(x, M_FLOAT)) - getfloat(as(y, M_FLOAT)));
        else 
            return mkint(getint(x) - getint(y));
    }
    return mkvoid();
})
OP('*', MUL, 2, M_NUM, (unit x, unit y) {
    if (is(x, M_NUM) && is(y, M_NUM)) {
        if (is(x, M_FLOAT) || is(y, M_FLOAT))
            return mkfloat(getfloat(as(x, M_FLOAT)) * getfloat(as(y, M_FLOAT)));
        else 
            return mkint(getint(x) * getint(y));
    }
    return mkvoid();
})
OP('/', DIV, 2, M_NUM, (unit x, unit y) {
    if (is(x, M_NUM) && is(y, M_NUM)) {
        if (is(x, M_FLOAT) || is(y, M_FLOAT))
            return mkfloat(getfloat(as(x, M_FLOAT)) / getfloat(as(y, M_FLOAT)));
        else 
            return mkint(getint(x) / getint(y));
    }
    return mkvoid();
})
OP('%', MOD, 2, M_NUM, (unit x, unit y) {
    if (is(x, M_NUM) && is(y, M_NUM)) {
        if (is(x, M_FLOAT) || is(y, M_FLOAT))
            return mkfloat(fmod(getfloat(as(x, M_FLOAT)), getfloat(as(y, M_FLOAT))));
        else 
            return mkint(getint(x) % getint(y));
    }
    return mkvoid();
})

OP('=', EQ, 2, M_INT, (unit x, unit y) {
    if (is(x, M_NUM) && is(y, M_NUM)) {
        if (is(x, M_FLOAT) || is(y, M_FLOAT))
            return mkint(getfloat(as(x, M_FLOAT)) == getfloat(as(y, M_FLOAT)));
        else 
            return mkint(getint(x) == getint(y));
    }
    return mkvoid();
})
OP('<', LT, 2, M_INT, (unit x, unit y) {
    if (is(x, M_NUM) && is(y, M_NUM)) {
        if (is(x, M_FLOAT) || is(y, M_FLOAT))
            return mkint(getfloat(as(x, M_FLOAT)) < getfloat(as(y, M_FLOAT)));
        else 
            return mkint(getint(x) < getint(y));
    }
    return mkvoid();
})
OP('>', GT, 2, M_INT, (unit x, unit y) {
    if (is(x, M_NUM) && is(y, M_NUM)) {
        if (is(x, M_FLOAT) || is(y, M_FLOAT))
            return mkint(getfloat(as(x, M_FLOAT)) > getfloat(as(y, M_FLOAT)));
        else 
            return mkint(getint(x) > getint(y));
    }
    return mkvoid();
})

OP('~', NOT, 1, M_INT, (unit x) {
    if (is(x, M_INT))
        return mkint(getint(x) == 0);
    else if (is(x, M_FLOAT))
        return mkint(getfloat(x) == 0.0);
    return mkvoid();
})

OP('&', AND, 2, M_ANY, (unit x, unit y) {
    if (is(x, M_VOID) 
        || (is(x, M_INT) && getint(x) == 0) 
        || (is(x, M_FLOAT) && getfloat(x) == 0.0) 
        || (is(x, M_ARR) && getlen(x) == 0))
        return x;
    else
        return y;
})
OP('|', OR, 2, M_ANY, (unit x, unit y) {
    if (is(x, M_VOID) 
        || (is(x, M_INT) && getint(x) == 0) 
        || (is(x, M_FLOAT) && getfloat(x) == 0.0) 
        || (is(x, M_ARR) && getlen(x) == 0))
        return y;
    else
        return x;
})

OP('.', PRINT, 1, M_VOID, (unit x) {
    if (is(x, M_INT))
        printf("%ld\n", getint(x));
    else if (is(x, M_FLOAT))
        printf("%f\n", getfloat(x));
    else if (is(x, M_STR))
        printf("%s\n", getstr(x));
    return mkvoid();
})

#endif
