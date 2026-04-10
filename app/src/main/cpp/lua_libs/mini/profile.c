/* clang-format off */
#include "mini.h" /* clang-format on */

#include "core/hooks.h"
#include "core/log.h"
#include "lua.h"
#include "events/profile.h"

#define LOG_TAG "MiniLuaProfile"

int getProfileID(lua_State *L) {
	if (g_profile_id == NULL)
		lua_pushnil(L);
	else
		lua_pushstring(L, g_profile_id);
	return 1;
}
