#include "mini_character.h"
#include "lua.h"
#include "lauxlib.h"


void miniLL_open_character(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, exp_functions);
	luaL_register(L, NULL, attrib_functions);
}

void initLL_character() {
	initLL_character_exp();
//	initLL_character_attrib();
}
