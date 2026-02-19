#include "config.h"

#include "global.h"
#include "log.h"

#include <android/asset_manager.h>
#include <stdlib.h>
#include <sys/types.h>

#define LOG_TAG "MiniConfig"

toml_table_t *miniC_open_asset(const char *asset_path) {
	char errbuf[200];

	// Open the asset, get file descriptor of it.
	AAsset *asset = AAssetManager_open(g_asset_mgr, asset_path, AASSET_MODE_BUFFER);
	if (asset == NULL) {
		LOGE("Attempted to load invalid Config from Assets: %s", asset_path);
		AAsset_close(asset);
		return NULL;
	}

	off_t asset_length = AAsset_getLength(asset);
	char *buffer = malloc(asset_length + 1);
	if (buffer == NULL) {
		LOGE("Failed to load Config Asset %s: OOM", asset_path);
		AAsset_close(asset);
		return NULL;
	}

	int read_bytes = AAsset_read(asset, buffer, asset_length);
	AAsset_close(asset);

	if (read_bytes != asset_length) {
		LOGE("Failed to read asset: Expected %li, got %i", asset_length, read_bytes);
		free(buffer);
		return NULL;
	}

	buffer[asset_length] = '\0';
	// We have the whole file in `buffer` now.

	toml_table_t *tbl = toml_parse(buffer, errbuf, sizeof(errbuf));
	if (tbl == NULL) {
		LOGE("%s", buffer);
		LOGE("TOML Error in %s: %s", asset_path, errbuf);
	}
	free(buffer);

	return tbl;
}

/** Load from given Mini Path. */
toml_table_t *miniC_open_file(const char *file_mini_path) {
	// TODO: Load from file?
	return NULL;
}

/** Tries to load config from Files or ExternalFiles, falls back to Assets. */
toml_table_t *miniC_open_config(const char *filename) {
	return NULL;
}
