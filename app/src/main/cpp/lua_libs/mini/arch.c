#include "lua.h"
#include "hooks.h"

int get_arch(lua_State *L) {
	lua_pushliteral(L, archSplit("armeabi-v7a", "arm64-v8a"));
	return 1;
}
