
#include <stddef.h>
#include "caver.h"
#include "symbol.h"
#include "log.h"
#include <string.h>

#define LOG_TAG "MiniCaverHook"

void *latestGameOverlayView = NULL;

DEFINE_HOOK_SYM(
        GameOverlayView_GameOverlayView,
        "_ZN5Caver15GameOverlayViewC1Ev",
        void, (void* this)
) {
    orig_GameOverlayView_GameOverlayView(this);
    latestGameOverlayView = this;
    LOGD("Caught new GOV: %p", latestGameOverlayView);
}

DEFINE_SYMBOL_SYM(
        GameOverlayView_SetControlsHidden,
        "_ZN5Caver15GameOverlayView17SetControlsHiddenEb",
        void, (void* this, bool hide)
)

const char* latestProfileId;

DEFINE_SYMBOL_HOOK_H(lfp, void*,
        (void *this, char **param_1, unsigned int param_2,
                long param_3, long param_4,
                long param_5, long param_6, long param_7))
DEFINE_HOOK_SYM(lfp, "_ZN5Caver13PlayerProfile12LoadFromPathERKSsb", void*,
                (void *this, char **param_1, unsigned int param_2,
                        long param_3, long param_4,
                        long param_5, long param_6, long param_7)) {
//    LOGD("Well ok lol %p, %p -> '%s'", this, param_1, *param_1);
//    LOGD("%ui, %p, %p, %p, %p, %p", param_2, param_3, param_4, param_5, param_6, param_7);

    latestProfileId = *(char**)(this + pointerOffset(3));

    LOGD("DEREF Test: '%s'", latestProfileId);

    return orig_lfp(this, param_1, param_2, param_3, param_4, param_5, param_6, param_7);
}

void setupCaverHooks() {
    LOGD("Setting up Caver hooks.");
    dlsym_GameOverlayView_SetControlsHidden();
    hook_GameOverlayView_GameOverlayView();

    hook_lfp();
}