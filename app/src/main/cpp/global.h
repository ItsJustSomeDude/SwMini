#ifndef NEEDLE_GLOBAL_H
#define NEEDLE_GLOBAL_H

#include <android/asset_manager.h>

/* This contains globals that are set from Java and may be needed across the C codebase. */

extern AAssetManager *g_asset_mgr;

/** App Private Files Dir, set from Java. Does NOT include trailing <code>/</code>! */
extern const char *g_ext_files;
/** App Private Cache Dir, set from Java. Does NOT include trailing <code>/</code>! */
extern const char *g_ext_cache;
/** App External Files Dir, set from Java. Does NOT include trailing <code>/</code>! */
extern const char *g_files_path;
/** App External Cache Dir, set from Java. Does NOT include trailing <code>/</code>! */
extern const char *g_cache_path;

/** This holds pointers to the global paths, for iterating over them. */
extern const char **g_path_list[4];

extern const char *g_cur_profile_id;
void miniG_set_profile_id(const char *id);

#endif //NEEDLE_GLOBAL_H
