#include "core/hooks.h"
#include "lua.h"

int miniLL_get_arch(lua_State *L) {
	lua_pushliteral(L, archSplit("armeabi-v7a", "arm64-v8a"));
	return 1;
}
