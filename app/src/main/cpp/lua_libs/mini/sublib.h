#ifndef NEEDLE_SUBLIB_H
#define NEEDLE_SUBLIB_H

#include "lua.h"
#include "lauxlib.h"

void miniL_register(lua_State *L, const char *lib_name, const luaL_Reg *functions);

#endif //NEEDLE_SUBLIB_H
