#ifndef SWMINI_PATCHES_H
#define SWMINI_PATCHES_H

#include <stdbool.h>

#ifdef __arm__
void setup32Patch();
#endif //__arm__

void setupAssetPatch();

void setup_string_hook(void);
void cleanup_string_hook(void);

extern bool allowNetwork;
void setupNetworkHooks(void);

void setup_panic_hook();

void init_lua_libs();

#endif //SWMINI_PATCHES_H
