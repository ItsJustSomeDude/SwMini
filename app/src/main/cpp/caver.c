
#include <stddef.h>
#include "caver.h"
#include "symbol.h"
#include "log.h"
#include <string.h>

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
        lfp,
        "_ZN5Caver13PlayerProfile12LoadFromPathERKSsb",
        void*,
        (void *this, char **p1, unsigned int p2, long p3, long p4, long p5, long p6, long p7)
) {
    latestProfileId = *(char**)(this + pointerOffset(3));
    LOGD("Fetched new Profile ID: %s", latestProfileId);
    return orig_lfp(this, p1, p2, p3, p4, p5, p6, p7);
}

void setupCaverHooks() {
    LOGD("Setting up Caver hooks.");
    dlsym_GameOverlayView_SetControlsHidden();
    hook_GameOverlayView_GameOverlayView();

    hook_lfp();
}