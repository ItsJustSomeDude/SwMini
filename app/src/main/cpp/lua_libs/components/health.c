#include "components.h"
#include "caver/components.h"

static int current_health(lua_State *L, void *component) {
	int *current_p = $(int, component, 0x0, 0x80);
	int *max_p = $(int, component, 0x0, 0x7c);

	if (!lua_isnil(L, -1)) {
		/* Set. */
		int desired = luaL_checkinteger(L, -1);
		if (desired > *max_p)
			*current_p = *max_p;
		else
			*current_p = desired;

		return 0;
	}

	lua_pushinteger(L, *current_p);
	return 1;
}

static int max_health(lua_State *L, void *component) {
	int *current_p = $(int, component, 0x0, 0x80);
	int *max_p = $(int, component, 0x0, 0x7c);

	if (!lua_isnil(L, -1)) {
		/* Set. */
		int desired = luaL_checkinteger(L, -1);
		if (*current_p > desired)
			*current_p = desired;

		*max_p = desired;
		return 0;
	}

	lua_pushinteger(L, *max_p);
	return 1;
}

static int health_string(lua_State *L, void *component) {
	CppString **str = $(CppString*, component, 0x0, 0x38);

	if (!lua_isnil(L, -1)) {
		/* Set. */
		CppString *n;
		CppString_create(&n, luaL_checkstring(L, -1));

		CppString_release(*str);

		*str = n;
		return 0;
	}

	lua_pushstring(L, *str);
	return 1;
}

const ComponentField HealthFields[] = {
	{"CurrentHealth", current_health},
	{"MaxHealth",     max_health},
	{"SomeString",    health_string},

	{NULL, NULL}
};
