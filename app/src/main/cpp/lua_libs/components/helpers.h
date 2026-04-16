#ifndef NEEDLE_HELPERS_H
#define NEEDLE_HELPERS_H

#include "lua.h"
#include "core/hooks.h"
#include "lauxlib.h"

#define SIMPLE_COMPONENT_FUNC(NAME, TYPE, O32, O64, GET_F, PUSH_F)  \
static int NAME(lua_State *L, void *component) {                    \
    TYPE *p = $(TYPE, component, O32, O64);                         \
    if (lua_isnil(L, -1)) {                                         \
        PUSH_F(L, *p);                                              \
        return 1;                                                   \
    }                                                               \
    *p = (TYPE) GET_F(L, -1);                                       \
    return 0;                                                       \
}

#endif //NEEDLE_HELPERS_H
