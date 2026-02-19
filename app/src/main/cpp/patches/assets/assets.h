#ifndef NEEDLE_ASSETS_H
#define NEEDLE_ASSETS_H

#include "libs/khash.h"

#include <android/asset_manager.h>
#include <linux/limits.h>
#include <stddef.h>

extern AAssetManager *asset_manager;

typedef enum AssetSource {
	APK = 1,
	Internal,
	External
} AssetSource;

typedef struct AssetEntry {
	void *buffer;
	size_t size;
	AssetSource source;
	/** The real path of the asset. If source is APK, it's an asset path, otherwise it's an absolute file path. */
	char path[PATH_MAX];
} AssetEntry;

KHASH_DECLARE(tbl, kh_cstr_t, AssetEntry*)

#endif //NEEDLE_ASSETS_H
