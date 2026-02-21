#include "assets.h"
#include "global.h"
#include "libs/khash.h"

#include <android/asset_manager.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>


static kh_tbl_t apk_assets;
static kh_tbl_t internal_assets;
static kh_tbl_t external_assets;

static kh_tbl_t internal_profile_assets;
static kh_tbl_t external_profile_assets;

void index_apk_assets() {
	// Loop through all the items in the `APK/assets/resources` folder, add to
	// index.

	AAssetDir *assetDir = AAssetManager_openDir(g_asset_mgr, "resources");
	const char *name = NULL;
	while ((name = AAssetDir_getNextFileName(assetDir)) != NULL) {
		AssetEntry *e = malloc(sizeof(AssetEntry));

		AAsset *asset =
			AAssetManager_open(g_asset_mgr, name, AASSET_MODE_STREAMING);
		size_t size = AAsset_getLength(asset);

		e->buffer = NULL;
		e->source = APK;
		e->size = size;

		strncpy(e->path, name, PATH_MAX - 1);
		e->path[PATH_MAX - 1] = '\0';

		AAsset_close(asset);
	}

	AAssetDir_close(assetDir);
}
