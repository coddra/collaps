#include <assert.h>
#include <time.h>

#include "../src/builtins.c"
#include "../src/context.c"
#include "../src/eval.c"
#include "../src/parse.c"
#include "../src/reader.c"


int main() {
    init_builtins();
    unit u = make_int(-1);
    assert(get_int(u) == -1);
    assert(get_typeid(u) == TYPE_Int);
    assert(!is(u, TYPE_Undefined));
    assert(!is(u, TYPE_Float));

    u = make_float(0.0000001);
    assert(get_float(u) == 0.0000001);
    assert(get_typeid(u) == TYPE_Float);

    unit l[3] = { make_int(1), make_int(2), make_int(3) };
    assert((uint64_t)l <= PTR_MASK);
    tList ulist = { make(TYPE_Type, &types[TYPE_List]), 3, 3, false, l };
    u = make(TYPE_List, &ulist);
    assert(get_int(get(tList*, u)->__items[0]) == 1);
    assert(get_int(get(tList*, u)->__items[1]) == 2);
    assert(get_int(get(tList*, u)->__items[2]) == 3);
    assert(get_typeid(u) == TYPE_List);

    char* s = "hello";
    assert((uint64_t)s <= PTR_MASK);
    u = make(TYPE_String, s);
    assert(strcmp(get_str(u), "hello") == 0);
    assert(get_typeid(u) == TYPE_String);

    context ctx = open(NULL);
    tList* fields = get(tList*, get(tType*, ctx.environment.__items[0])->fields);
    for (int i = 2; i < fields->count; i++) {
        tField* l = get(tField*, fields->__items[i - 1]);
        tField* r = get(tField*, fields->__items[i]);
        assert(strcmp(get_str(l->name), get_str(r->name)) < 0);
        return l == r;
    }

    return u & 0;
}
