
#ifndef SWMINI_CAVER_H
#define SWMINI_CAVER_H

#include <stdbool.h>
#include "symbol.h"

void setupCaverHooks();

DEFINE_SYMBOL_HOOK_H(
        GameOverlayView_GameOverlayView,
        void, (void* this)
)

extern void *latestGameOverlayView;
extern const char *latestProfileId;

H_DL_FUNCTION(
        GameOverlayView_SetControlsHidden,
        void, (void* this, bool hide)
)


#endif //SWMINI_CAVER_H
