/* clang-format off */
#include "mini.h" /* clang-format on */

#include "features/coin_limit.h"
#include "hooks.h"
#include "log.h"
#include "lua.h"

#include <stdlib.h>
#include <string.h>

#define LOG_TAG "MiniLuaProfile"

const char *latestProfileId;

STATIC_DL_HOOK_SYMBOL(
	loadProfileId,
	"_ZN5Caver22MainMenuViewController38ProfileSelectionViewControllerDidStartEPNS_20ProfileSelectionViewERKN5boost10shared_ptrINS_13PlayerProfileEEE",
	void*,
	(void *this, void* p1, void** profile)
) {
	const char *profileId = *$(char*, *profile, (3 * sizeof(void *)), (3 * sizeof(void *)));
	if (latestProfileId != NULL) {
		// Free the old string copy.
		free((void *) latestProfileId);
	}
	latestProfileId = strdup(profileId);
	LOGD("Fetched new Profile ID: %s", latestProfileId);

	// Since we're entering a new Save file, reset the Coin Limit in case it was modified from Lua.
	miniCL_reset();

	return orig_loadProfileId(this, p1, profile);
}

int getProfileID(lua_State *L) {
	if (latestProfileId == NULL) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushlstring(L, latestProfileId, strlen(latestProfileId));
	return 1;
}

void init_profileId() {
	LOGD("Setup ProfileID hook.");
	hook_loadProfileId();
}
