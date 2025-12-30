#ifndef SWMINI_MINI_LUA_LIBRARY_H
#define SWMINI_MINI_LUA_LIBRARY_H

#include "../hooks.h"
#include "lua.h"
#include "lauxlib.h"

#define MINI_LIB_NAME "Mini"
extern const luaL_Reg minilib[];
int open_mini(lua_State *L);
void init_mini_lua_lib();

void init_setControlsHidden();
int setControlsHidden(lua_State *L);

void init_profileId();
int getProfileID(lua_State *L);
extern const char *latestProfileId;

int get_arch(lua_State *L);


#endif //SWMINI_MINI_LUA_LIBRARY_H
