#include "components.h"

static int swing_string1(lua_State *L, void *component) {
	CppString **str = $(CppString*, component, 0x0, 0x88);

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

static int swing_string2(lua_State *L, void *component) {
	CppString **str = $(CppString*, component, 0x0, 0x90);

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


const ComponentField SwingableWeaponControllerFields[] = {
	{"SomeString",  swing_string1},
	{"SomeString2", swing_string2},

	{NULL, NULL}
};
