#include "backend.h"

#include "core/log.h"
#include "core/assets.h"
#include "core/impl_files/mini_file.h"
#include "core/files/dirent.h"

#include <malloc.h>
#include <errno.h>
#include <android/asset_manager.h>
#include <string.h>
#include <dirent.h>

#define LOG_TAG "MiniDirent"

MiniDIR *_Nullable assets_opendir(const char *path_string) {
	AAssetDir *dir = AAssetManager_openDir(g_asset_mgr, path_string);
	if (dir == NULL) {
		errno = ENOENT;
		return NULL;
	}

	MiniDIR *out = malloc(sizeof(MiniDIR));
	if (out == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	out->assets_dir = dir;
	out->backend = &backend_assets;

	/* Fill structure with empty data. */
	out->dirent.d_name[0] = '\0';

	return out;
}

mini_dirent *_Nullable assets_readdir(MiniDIR *_Nonnull dir) {
	const char *name = AAssetDir_getNextFileName(dir->assets_dir);
	if (name == NULL) {
		/* End of files in dir. Don't touch errno. */
		dir->dirent.d_name[0] = '\0';
		return NULL;
	} else {
		strncpy(dir->dirent.d_name, name, sizeof(dir->dirent.d_name));
	}
	return &dir->dirent;
}

int assets_closedir(MiniDIR *_Nonnull dir) {
	AAssetDir_close(dir->assets_dir);
	free(dir);
	return 0;
}
