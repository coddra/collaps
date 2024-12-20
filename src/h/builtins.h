#ifndef _BUILTINS_H
#define _BUILTINS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "util.h"

#define TYPE_(name) CAT(TYPE_, name)
#define OP_(name) CAT(OP_, name)
#define FUNC_(name) CAT(FUNC_, name)

enum TYPE {
#   define ZTYPE TYPE
#   define ATYPE TYPE
#   define TYPE(name, ...) TYPE_(name),
#      include "builtindefs.h"
#   undef ZTYPE
#   undef ATYPE
#   undef TYPE
    TYPE_COUNT
};
enum OP {
#   define OP(key, name, ...) OP_(name),
#       include "builtindefs.h"
#   undef OP
    OP_COUNT
};
enum FUNC {
#   define FUNC(name, ...) CAT(FUNC_, name),
#       include "builtindefs.h"
#   undef FUNC
    FUNC_COUNT
};

#define MAX_ARGC 16
#define OP_MAX_LENGTH 3

typedef uint64_t unit;
// Must define here to avoid circular dependencies
typedef struct context context;

#define FLD(name, readonly) unit name;
#define HIDDEN(...) __VA_ARGS__;
#define ZTYPE(name)
#define ATYPE(name, base) typedef base CAT(t, name);
#define TYPE(name, parent, fields) typedef struct { unit __type; fields } CAT(t, name);
#define OP(key, name, argc, ...) unit CAT(o, name)(context* ctx, unit* args);
#define FUNC(name, argc, ...) unit CAT(f, name)(context* ctx, unit* args);
#   include "builtindefs.h"
#undef FLD
#undef HIDDEN
#undef ZTYPE
#undef ATYPE
#undef TYPE
#undef OP
#undef FUNC

extern tType types[TYPE_COUNT];
extern tFunc ops[OP_COUNT];
extern tFunc funcs[FUNC_COUNT];

extern const unit vUndefined;
extern const unit vTrue;
extern const unit vFalse;

typedef union {
    int64_t i;
    uint64_t u;
    bool b;
    double d;
    const char* s;
    void* p;
} uc;

unit make(enum TYPE type, uc u);
unit make_alloc(enum TYPE type, void* p);
unit mklistalloc(tList l);
unit mkfieldalloc(tField f);

#define PTR_WIDTH 48ul // on x86-64, user space adresses always fit on 48 bits
#define PTR_MASK ((1ul << PTR_WIDTH) - 1)
#define get(type, u) ((type)((unit)(u) & PTR_MASK))

size_t size_of(enum TYPE type);

bool is(unit u, enum TYPE type);

void init_builtins();

// TODO: Methods
tList tList_new();
void tList_insert(tList* l, unit item, size_t index);
static inline void push(tList* l, unit item) { tList_insert(l, item, l->count); }
void tList_drop(tList* l, size_t n);
unit tFunc_invoke(context* ctx, tFunc f, ...);
int tString_compare(tString* l, tString* r);

#endif
