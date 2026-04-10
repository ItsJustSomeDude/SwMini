#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include "profile.h"
#include "core/log.h"
#include "features/coin_limit.h"
#include "core/hooks.h"
#include "init/events.h"
#include "core/impl_files/resource_paths.h"

#define LOG_TAG "MiniEventProfile"

char *g_profile_id;

void miniEV_profile_changed(const char *id) {
	LOGD("Fetched new Profile ID: %s", g_profile_id);

	/* Update stored Profile ID, freeing old if needed. */
	if (g_profile_id != NULL)
		free(g_profile_id);
	g_profile_id = strdup(id);

	/* Since we're entering a new Save file, reset the Coin Limit in case it was modified from Lua. */
	miniCL_reset();

	/* Update the Asset paths with the current profile id. */
	miniR_refresh_paths();
}

STATIC_DL_HOOK_SYMBOL(
	loadProfileId,
	"_ZN5Caver22MainMenuViewController38ProfileSelectionViewControllerDidStartEPNS_20ProfileSelectionViewERKN5boost10shared_ptrINS_13PlayerProfileEEE",
	void*, (void *this, void* p1, void** profile)
) {
	const size_t offset = 3 * sizeof(void *);
	const char *profileId = *$(char*, *profile, offset, offset);

	miniEV_profile_changed(profileId);

	return orig_loadProfileId(this, p1, profile);
}

void initEV_profile(void) {
	hook_loadProfileId();
}
