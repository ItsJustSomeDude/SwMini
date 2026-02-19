#include "hooks.h"
#include "lauxlib.h"
#include "log.h"
#include "lua.h"
#include "lua_libs/lfs.h"
#include "lua_libs/mini/mini.h"
#include "lualib.h"
#include "luasocket/src/luasocket.h"
#include "patches.h"

#include <stddef.h>

#define LOG_TAG "MiniLuaLibs"

static const luaL_Reg lualibs[] = {
	// We cannot load base, it gets loaded by vanilla.
	// {"", luaopen_base},

	// Not adding package for now.
	{LUA_LOADLIBNAME, luaopen_package},

	{LUA_TABLIBNAME,  luaopen_table},
	{LUA_IOLIBNAME,   luaopen_io},
	{LUA_OSLIBNAME,   luaopen_os},

	// We cannot load string this way, because it's bundled in LS.
	// {LUA_STRLIBNAME, luaopen_string},

	{LUA_MATHLIBNAME, luaopen_math},
	{LUA_DBLIBNAME,   luaopen_debug},

	// Mini!
	// {MINI_LIB_NAME,   miniL_open_mini},

	{NULL, NULL}
};

void luaL_openlibs(lua_State *L) {
	const luaL_Reg *lib = lualibs;
	for (; lib->func; lib++) {
		lua_pushcfunction(L, lib->func);
		lua_pushstring(L, lib->name);
		lua_call(L, 1, 0);
	}

	lua_pushcfunction(L, luaopen_base);
	lua_pushstring(L, "");
	lua_call(L, 1, 0);

	lua_pushcfunction(L, luaopen_string);
	lua_pushstring(L, LUA_STRLIBNAME);
	lua_call(L, 1, 0);
}


STATIC_DL_HOOK_ADDR(openString, luaopen_string, void, (lua_State * L)) {
	LOGD("Loading all Patched Lua Libraries into lua_State %p", L);

	const luaL_Reg *lib = lualibs;
	for (; lib->func; lib++) {
		lua_pushcfunction(L, lib->func);
		lua_pushstring(L, lib->name);
		lua_call(L, 1, 0);
	}

	lua_pushcfunction(L, miniL_open_mini);
	lua_pushstring(L, MINI_LIB_NAME);
	lua_call(L, 1, 0);

	luaopen_socket_core(L);
	lua_setglobal(L, "broken_socket");

	luaopen_lfs(L);

	// openString is the function we hooked, so run the original.
	orig_openString(L);

// TODO: Possible Caver bug? An item is left on the stack, check for crashes. Mini pops it, but this might not be a good thing.
// lua_pop(L, 1);
}

void init_patch_lua_libs() {
	LOGD("Applying Lua Library Loading patch, at address %p", luaopen_string);

	hook_openString();
}
