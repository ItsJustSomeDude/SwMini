#ifndef NEEDLE_LUA_LIBS_H
#define NEEDLE_LUA_LIBS_H

#include "lua/lua.h"

/* Global Libs */

#define MINI_LIB_NAME "Mini"
int miniLL_open_mini(lua_State *L);
int miniLL_open_lni(lua_State *L);
int miniLL_open_components(lua_State *L);


/* Mini sub-libs. The only difference is what table they're placed in inside `program_state.c` */
int miniLL_open_health(lua_State *L);
int miniLL_open_character(lua_State *L);


#endif /* NEEDLE_LUA_LIBS_H */
