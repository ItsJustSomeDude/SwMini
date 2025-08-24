
#ifndef SWMINI_CAVER_H
#define SWMINI_CAVER_H

#include <stdbool.h>
#include "symbol.h"

void setupCaverHooks();

extern void *latestGameOverlayView;
extern const char *latestProfileId;

H_DL_FUNCTION(
        GameOverlayView_SetControlsHidden,
        void, (void* this, bool hide)
)


#endif //SWMINI_CAVER_H
