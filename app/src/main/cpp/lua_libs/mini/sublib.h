#ifndef NEEDLE_SUBLIB_H
#define NEEDLE_SUBLIB_H

#include "lauxlib.h"
#include "lua.h"

void miniL_register(lua_State *L, const char *lib_name, const luaL_Reg *functions);

#endif //NEEDLE_SUBLIB_H
