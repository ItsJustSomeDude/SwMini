/* clang-format off */
#include "mini.h" /* clang-format on */

#include "features/coin_limit.h"
#include "hooks.h"
#include "log.h"
#include "lua.h"
#include "global.h"

#include <stdlib.h>
#include <string.h>

#define LOG_TAG "MiniLuaProfile"

STATIC_DL_HOOK_SYMBOL(
	loadProfileId,
	"_ZN5Caver22MainMenuViewController38ProfileSelectionViewControllerDidStartEPNS_20ProfileSelectionViewERKN5boost10shared_ptrINS_13PlayerProfileEEE",
	void*,
	(void *this, void* p1, void** profile)
) {
	const char *profileId = *$(char*, *profile, (3 * sizeof(void *)), (3 * sizeof(void *)));
	miniG_set_profile_id(profileId);

	return orig_loadProfileId(this, p1, profile);
}

int getProfileID(lua_State *L) {
	if (g_cur_profile_id == NULL) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushlstring(L, g_cur_profile_id, strlen(g_cur_profile_id));
	return 1;
}

void init_profileId() {
	LOGD("Setup ProfileID hook.");
	hook_loadProfileId();
}
