#include "core/hooks.h"
#include "lauxlib.h"
#include "core/log.h"
#include "lualib.h"
#include "lua_libs/libs.h"

/* External stuff... */
#include "libs/luasocket/src/luasocket.h"
#include "lua_libs/lfs/lfs.h"

#define LOG_TAG "MiniEventLuaInit"

/* This fires when a new Lua State is initialized for use in Scenes. */

static const luaL_Reg global_libs[] = {
	{MINI_LIB_NAME,   miniLL_open_mini},
	{"LNI",           miniLL_open_lni},
	{"Components",    miniLL_open_components},

	/* External stuff again. */
	{"broken_socket", luaopen_socket_core},
	{"fs",            luaopen_lfs},

	{NULL, NULL}
};

static const luaL_Reg mini_sub_libs[] = {
	{"Health",    miniLL_open_health},
	{"Character", miniLL_open_character},
	{NULL, NULL}
};

typedef struct ProgramState {
	lua_State *L;
	/* There's a lot more to this, but L is first. */
} ProgramState;

STATIC_DL_HOOK_SYMBOL(
	register_libraries, "_ZN5Caver12ProgramState22RegisterProgramLibraryEv",
	void, (ProgramState * this)
) {
	orig_register_libraries(this);
	lua_State *L = this->L;
	LOGD("Loading all Lua Libraries into new ProgramState %p", this);

	/* All the vanilla Lua libs. */
	luaL_open_ext_libs(L);

	/* Push all the global libs to _G */
	const luaL_Reg *glib = global_libs;
	for (; glib->name; glib++) {
		glib->func(L);
		lua_setglobal(L, glib->name);
	}

	/* Mini's sublibraries, inserted in the `Mini` table. */
	lua_getfield(L, LUA_GLOBALSINDEX, MINI_LIB_NAME);
	const luaL_Reg *lib = mini_sub_libs;
	for (; lib->name; lib++) {
		lib->func(L);
		lua_setfield(L, -2, lib->name);
	}
}

void initP_program_state(void) {
	LOGD("Patching ProgramState init to insert all Lua Libraries.");

	hook_register_libraries();
}