/*
** $Id: linit.c,v 1.14.1.1 2007/12/27 13:02:25 roberto Exp $
** Initialization of libraries for lua.c
** See Copyright Notice in lua.h
*/


#define linit_c
#define LUA_LIB

#include "lua.h"

#include "lualib.h"
#include "lauxlib.h"


static const luaL_Reg lualibs[] = {
	/* {"", luaopen_base}, */
	{LUA_LOADLIBNAME, luaopen_package},
	{LUA_TABLIBNAME,  luaopen_table},
	{LUA_IOLIBNAME,   luaopen_io},
	{LUA_OSLIBNAME,   luaopen_os},
	/* {LUA_STRLIBNAME, luaopen_string}, */
	{LUA_MATHLIBNAME, luaopen_math},
	{LUA_DBLIBNAME,   luaopen_debug},
	{NULL, NULL}
};

/* This needed to be added by Mini, because it's not in the engine. string and base are different
 * as well. It doesn't get used by Mini, but may be needed by rocks. */

LUALIB_API void luaL_openlibs(lua_State *L) {
	lua_pushcfunction(L, luaopen_base);
	lua_pushstring(L, "");
	lua_call(L, 1, 0);

	lua_pushcfunction(L, luaopen_string);
	lua_pushstring(L, LUA_STRLIBNAME);
	lua_call(L, 1, 0);

	luaL_open_ext_libs(L);
}

LUALIB_API void luaL_open_ext_libs(lua_State *L) {
	const luaL_Reg *lib = lualibs;
	for (; lib->func; lib++) {
		lua_pushcfunction(L, lib->func);
		lua_pushstring(L, lib->name);
		lua_call(L, 1, 0);
	}
}