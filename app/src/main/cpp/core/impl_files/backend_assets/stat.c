#include "backend.h"

#include "core/assets.h"
#include "core/files/stat.h"
#include "core/log.h"

#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <android/asset_manager.h>

#define LOG_TAG "MiniAssetsStat"

int assets_stat(const char *path_string, mini_stat *_Nonnull buf) {
	memset(buf, 0, sizeof(*buf));

	AAsset *asset = AAssetManager_open(g_asset_mgr, path_string, AASSET_MODE_STREAMING);
	if (asset == NULL)
		return -1;

	off_t len = AAsset_seek(asset, 0, SEEK_END);
	AAsset_close(asset);

	buf->st_size = len;
	buf->st_mode = S_IFREG | 0444; /* regular file, read-only */

	return 0;
}

int assets_lstat(const char *path_string, mini_stat *_Nonnull buf) {
	/* No symlinks in assets. */
	errno = EOPNOTSUPP;
	return -1;
}

int assets_mkdir(const char *path_string, mode_t mode) {
	/* Assets are readonly. */
	errno = EROFS;
	return -1;
}
