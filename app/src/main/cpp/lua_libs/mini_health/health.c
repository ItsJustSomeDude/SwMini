
#include "caver/components.h"
#include "caver/program_state.h"
#include "caver/scene_object.h"
#include "core/hooks.h"
#include "lauxlib.h"

#include <stddef.h>

#define LOG_TAG "MiniLuaHealth"

static int current_mana(lua_State *L) {
	void *prog_state = program_state_from_L(L);
	SceneObject **object = lua_touserdata(L, 1);

	// Bail out if object is null.
	if (object == NULL || *object == NULL)
		return 0;

	void *mana_component = SceneObject_ComponentWithInterface(*object, ManaComponent_Interface);
	if (mana_component == NULL)
		return 0;

	int max_mana = *$(int, mana_component, 0x3c, 0x70);
	int current_mana = *$(int, mana_component, 0x40, 0x74);

	lua_pushinteger(L, current_mana);
	return 1;
}

static int current_mana_percent(lua_State *L) {
	void *prog_state = program_state_from_L(L);
	SceneObject **object = lua_touserdata(L, 1);

	// Bail out if object is null.
	if (object == NULL || *object == NULL)
		return 0;

	void *mana_component = SceneObject_ComponentWithInterface(*object, ManaComponent_Interface);
	if (mana_component == NULL)
		return 0;

	int max_mana = *$(int, mana_component, 0x3c, 0x70);
	int current_mana = *$(int, mana_component, 0x40, 0x74);

	lua_pushnumber(L, (double) current_mana / max_mana);
	return 1;
}

static const luaL_Reg health_lib[] = {
	{"CurrentMana",        current_mana},
	{"CurrentManaPercent", current_mana_percent},
	{NULL, NULL}
};

void miniLL_open_health(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, health_lib);
}
