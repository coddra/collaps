#include <assert.h>
#include "../src/h/unit.h"

#include "../src/reader.c"
#include "../src/eval.c"
#include "../src/parse.c"


int main() {
    unit u = mkint(-1);
    assert(getint(u) == -1);
    assert(gettype(u) == T_INT);
    assert(!is(u, T_VOID));
    assert(!is(u, T_FLOAT));

    u = mkfloat(0.0000001);
    assert(getfloat(u) == 0.0000001);
    assert(gettype(u) == T_FLOAT);

    unit l[3] = { mkint(1), mkint(2), mkint(3) };
    assert((uint64_t)l <= PTR_MASK);
    list ulist = { 3, l };
    u = mklist(&ulist);
    assert(getint(getlist(u)->items[0]) == 1);
    assert(getint(getlist(u)->items[1]) == 2);
    assert(getint(getlist(u)->items[2]) == 3);
    assert(gettype(u) == T_LIST);

    char *s = "hello";
    assert((uint64_t)s <= PTR_MASK);
    u = mkstr(s);
    assert(strcmp(getstr(u), "hello") == 0);
    assert(gettype(u) == T_STR);

    for (int i = 1; i < OP_COUNT; i++) {
        assert(strcmp(ops[i].name, ops[i - 1].name) > 0);
    }
    return !(getstr(u) == s);
}
