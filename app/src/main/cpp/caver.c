
#include <stddef.h>
#include "caver.h"
#include "symbol.h"
#include "log.h"
#include <string.h>
#include <malloc.h>

#define LOG_TAG "MiniCaverHook"

void *latestGameOverlayView = NULL;

STATIC_DL_HOOK_SYMBOL(
        GameOverlayView_GameOverlayView,
        "_ZN5Caver15GameOverlayViewC1Ev",
        void, (void* this)
) {
    orig_GameOverlayView_GameOverlayView(this);
    latestGameOverlayView = this;
    LOGD("Caught new GOV: %p", latestGameOverlayView);
}

DL_FUNCTION_SYMBOL(
        GameOverlayView_SetControlsHidden,
        "_ZN5Caver15GameOverlayView17SetControlsHiddenEb",
        void, (void* this, bool hide)
)

const char* latestProfileId;

STATIC_DL_HOOK_SYMBOL(
        loadProfile,
        "_ZN5Caver22MainMenuViewController38ProfileSelectionViewControllerDidStartEPNS_20ProfileSelectionViewERKN5boost10shared_ptrINS_13PlayerProfileEEE",
        void*,
        (void *this, void* p1, void** profile)
) {
    const char* profileId = *(char**)(*profile + pointerOffset(3));
    if(latestProfileId != NULL) {
        // Free the old string copy.
        free((void*) latestProfileId);
    }
    latestProfileId = strdup(profileId);
    LOGD("Fetched new Profile ID: %s", latestProfileId);
    return orig_loadProfile(this, p1, profile);
}

void setupCaverHooks() {
    LOGD("Setting up Caver hooks.");
    dlsym_GameOverlayView_SetControlsHidden();
    hook_GameOverlayView_GameOverlayView();

    hook_loadProfile();
}