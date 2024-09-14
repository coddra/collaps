#include <assert.h>

#include "../src/builtins.c"
#include "../src/eval.c"
#include "../src/parse.c"
#include "../src/reader.c"


int main() {
    init_builtins();
    unit u = mkint(-1);
    assert(getint(u) == -1);
    assert(gettypeid(u) == TYPE_Int);
    assert(!is(u, TYPE_Undefined));
    assert(!is(u, TYPE_Float));

    u = mkfloat(0.0000001);
    assert(getfloat(u) == 0.0000001);
    assert(gettypeid(u) == TYPE_Float);

    unit l[3] = { mkint(1), mkint(2), mkint(3) };
    assert((uint64_t)l <= PTR_MASK);
    tList ulist = { 3, 3, false, l };
    u = make(TYPE_List, &ulist);
    assert(getint(get(tList*, u)->__items[0]) == 1);
    assert(getint(get(tList*, u)->__items[1]) == 2);
    assert(getint(get(tList*, u)->__items[2]) == 3);
    assert(gettypeid(u) == TYPE_List);

    char* s = "hello";
    assert((uint64_t)s <= PTR_MASK);
    u = make(TYPE_String, s);
    assert(strcmp(getstr(u), "hello") == 0);
    assert(gettypeid(u) == TYPE_String);
    assert(is(u, TYPE_Object));

    for (int i = 1; i < OP_COUNT; i++)
        assert(strcmp(getstr(ops[i].name), getstr(ops[i - 1].name)) > 0);
    for (int i = 1; i < FUNC_COUNT; i++)
        assert(strcmp(getstr(funcs[i].name), getstr(funcs[i - 1].name)) > 0);
    for (int i = 2; i < TYPE_COUNT; i++)
        assert(strcmp(getstr(types[i].name), getstr(types[i - 1].name)) > 0);
}
