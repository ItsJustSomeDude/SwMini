#include "health.h"

#include "../sublib.h"
#include "caver/component.h"
#include "caver/program_state.h"
#include "caver/scene_object.h"
#include "hooks.h"
#include "lauxlib.h"

#include <stddef.h>

#define LOG_TAG "MiniLuaHealth"

STATIC_DL_FUNCTION_SYMBOL(
	mana_component_interface,
	"_ZN5Caver13ManaComponent9InterfaceEv",
	void, (void)
)

static int current_mana(lua_State *L) {
	void *prog_state = program_state_from_L(L);
	SceneObject **object = lua_touserdata(L, 1);

	// Bail out if object is null.
	if (object == NULL || *object == NULL)
		return 0;

	void *mana_component = ComponentWithInterface(*object, mana_component_interface);
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

	void *mana_component = ComponentWithInterface(*object, mana_component_interface);
	if (mana_component == NULL)
		return 0;

	int max_mana = *$(int, mana_component, 0x3c, 0x70);
	int current_mana = *$(int, mana_component, 0x40, 0x74);

	lua_pushnumber(L, (double) current_mana / max_mana);
	return 1;
}

#define MINI_HEALTH_LIB_NAME "Health"
static const luaL_Reg health_lib[] = {
	{"CurrentMana",        current_mana},
	{"CurrentManaPercent", current_mana_percent},
	{NULL, NULL}
};

void miniL_open_health(lua_State *L) {
	miniL_register(L, MINI_HEALTH_LIB_NAME, health_lib);
}

void init_lua_health() {
	dlsym_mana_component_interface();
}
