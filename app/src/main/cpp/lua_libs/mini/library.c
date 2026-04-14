#include "../lni/lni.h"
#include "features/coin_limit.h"
#include "lauxlib.h"
#include "lua.h"
#include "luaconf.h"
#include "mini.h"

#include <stddef.h>

static const luaL_Reg mini_lib[] = {
	{"SetControlsHidden", miniLL_set_controls_hidden},
	{"GetProfileID",      miniLL_get_profile_id},
	{"Arch",              miniLL_get_arch},
//	{"ExecuteLNI",        lni_execute},
//	{"BindLNI",           lni_bind},

//	{"SceneFindAll",      scene_find_all},

//	{"ToggleDebug",       miniLL_toggle_debug},

	{"SetCoinLimit",      miniCL_set_from_lua},

	{"RecreateHero",      miniLL_recreate_hero},

	{"map",               miniLL_the_map_function},

	{NULL, NULL}
};

LUALIB_API int miniLL_open_mini(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, mini_lib);

	return 1;
}
