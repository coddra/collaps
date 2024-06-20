#include <assert.h>
#include "../src/unit.h"


int main() {
    unit u = mkint(-1);
    assert(getint(u) == -1);
    assert(gettype(u) == M_INT);
    assert(!is(u, M_VOID));
    assert(is(u, M_NUM));
    assert(!is(u, M_FLOAT));

    u = mkfloat(0.0000001);
    assert(getfloat(u) == 0.0000001);
    assert(gettype(u) == M_FLOAT);

    unit list[3] = { mkint(1), mkint(2), mkint(3) };
    assert((uint64_t)list <= PTR_MASK);
    u = mklist(list, 3);
    assert(getlen(u) == 3);
    assert(getint(getlist(u)[0]) == 1);
    assert(getint(getlist(u)[1]) == 2);
    assert(getint(getlist(u)[2]) == 3);
    assert(gettype(u) == M_LIST);

    char *s = "hello";
    assert((uint64_t)s <= PTR_MASK);
    u = mkstr(s);
    assert(strcmp(getstr(u), "hello") == 0);
    assert(getlen(u) == 5);
    assert(gettype(u) == M_STR);
}
