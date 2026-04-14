#ifndef SWMINI_MINI_LUA_LIBRARY_H
#define SWMINI_MINI_LUA_LIBRARY_H

#include "core/hooks.h"
#include "lauxlib.h"
#include "lua.h"

int miniLL_set_controls_hidden(lua_State *L);
int miniLL_get_profile_id(lua_State *L);
int miniLL_toggle_debug(lua_State *L);
int miniLL_recreate_hero(lua_State *L);
int miniLL_get_arch(lua_State *L);
int miniLL_the_map_function(lua_State *L);

#endif //SWMINI_MINI_LUA_LIBRARY_H
