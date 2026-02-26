#ifndef SWMINI_PATCHES_H
#define SWMINI_PATCHES_H

#include <stdbool.h>

#ifdef __arm__
void init_patch_32bit();
#endif //__arm__

void init_patch_assets();

extern bool allow_network_access;
void init_patch_network(void);

void init_patch_panic();

void init_patch_lua_libs();
void initP_lua_load_file();

#endif //SWMINI_PATCHES_H
