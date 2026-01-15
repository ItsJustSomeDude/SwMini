#include "lua.h"

void register_lni_global(lua_State *L) {
	lua_getglobal(L, "Mini");
	lua_getfield(L, -1, "BindLNI");

	lua_newtable(L);


}
