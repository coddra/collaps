#include <assert.h>

#include "../src/builtins.c"
#include "../src/eval.c"
#include "../src/parse.c"
#include "../src/reader.c"


int main() {
    init_builtins();
    unit u = mkint(-1);
    assert(getint(u) == -1);
    assert(gettype(u) == TYPE_Int);
    assert(!is(u, TYPE_Void));
    assert(!is(u, TYPE_Float));

    u = mkfloat(0.0000001);
    assert(getfloat(u) == 0.0000001);
    assert(gettype(u) == TYPE_Float);

    unit l[3] = { mkint(1), mkint(2), mkint(3) };
    assert((uint64_t)l <= PTR_MASK);
    tList ulist = { 3, 3, false, l };
    u = mklist(&ulist);
    assert(getint(getlist(u)->__items[0]) == 1);
    assert(getint(getlist(u)->__items[1]) == 2);
    assert(getint(getlist(u)->__items[2]) == 3);
    assert(gettype(u) == TYPE_List);

    char *s = "hello";
    assert((uint64_t)s <= PTR_MASK);
    u = mkstr(s);
    assert(strcmp(getstr(u), "hello") == 0);
    assert(gettype(u) == TYPE_String);

    for (int i = 1; i < OP_COUNT; i++)
        assert(strcmp(getstr(ops[i].name), getstr(ops[i - 1].name)) > 0);
    for (int i = 1; i < FUNC_COUNT; i++)
        assert(strcmp(getstr(funcs[i].name), getstr(funcs[i - 1].name)) > 0);

    return !(getstr(u) == s);
}
