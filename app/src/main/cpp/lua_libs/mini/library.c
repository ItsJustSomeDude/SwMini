#include "../lni/lni.h"
#include "features/coin_limit.h"
#include "health/health.h"
#include "lauxlib.h"
#include "lua.h"
#include "luaconf.h"
#include "mini.h"

#include <stddef.h>

static int lua_coin_limit(lua_State *L) {
	lua_Integer n = luaL_checkinteger(L, 1);

	if (n < 0 || n > 0xFFFF)
		return luaL_error(L, "Coin Limit must be 0–65535");

	unsigned short limit = (unsigned short) n;
	miniCL_set(limit);

	return 0;
}

static const luaL_Reg mini_lib[] = {
	{"SetControlsHidden", setControlsHidden},
	{"GetProfileID",      getProfileID},
	{"Arch",              get_arch},
	{"ExecuteLNI",        lni_execute},
	{"BindLNI",           lni_bind},

//	{"SceneFindAll",      scene_find_all},

//	{"ToggleDebug",       tdb},

	{"SetCoinLimit",      lua_coin_limit},

	{"RecreateHero",      recreate_hero},

//{ "TestGOV", test },
//{ "TestGAO", test2 },

	{NULL, NULL}
};

LUALIB_API int miniL_open_mini(lua_State *L) {
	// TODO: Move to better location.
	lni_bind_global(L, "copyToClipboard", 4, "Copy", "copy", "CopyToClipboard", "copyToClipboard");
	lni_bind_global(L, "openUrl", 4, "openUrl", "openURL", "OpenUrl", "OpenURL");
	lni_bind_global(L, "setSpeed", 2, "setSpeed", "SetSpeed");
	lni_bind_global(L, "getSpeed", 2, "getSpeed", "GetSpeed");
	lni_bind_global(L, "quit", 2, "Quit", "quit");

	luaL_register(L, MINI_LIB_NAME, mini_lib);

	miniL_open_health(L);

	return 1;
}

void init_mini_lua_lib() {
	init_setControlsHidden();
	init_profileId();
	init_scene_find_all();
	init_lua_debug();
	init_lua_recreate_hero();

	init_lua_health();
}
