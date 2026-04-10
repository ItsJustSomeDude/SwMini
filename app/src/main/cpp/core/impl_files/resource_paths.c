#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <android/asset_manager.h>

#include "core/log.h"
#include "resource_paths.h"
#include "events/profile.h"
#include "core/impl_files/paths.h"
#include "core/assets.h"
#include "path_parser.h"
#include "core/impl_files/backend_std/backend.h"
#include "core/impl_files/backend_assets/backend.h"

#define LOG_TAG "MiniResourcePaths"

/* These logs are very chatty, turn them off. */
//#undef LOGD
//#define LOGD(...) ((void)0)

ResPaths *global_res_paths;

void miniR_refresh_paths(void) {
	/* Create a new Global ResPath block, and replace the old one. */

	/* First, build all the paths based on current information. */
	char profile_external_path[MINI_PATH_SIZE] = {0};
	if (g_profile_id != NULL && g_ext_files != NULL) {
		/* g_ext_files + /assets/ + profile_id + / */
		snprintf(profile_external_path, MINI_PATH_SIZE, "%s%s%s/",
		         g_ext_files, EXT_RESOURCES_SUBDIR, g_profile_id);
	}

	char profile_internal_path[MINI_PATH_SIZE] = {0};
	if (g_profile_id != NULL && g_files_path != NULL) {
		/* g_files_path + /assets/ + profile_id + / */
		snprintf(profile_internal_path, MINI_PATH_SIZE, "%s%s%s/",
		         g_files_path, EXT_RESOURCES_SUBDIR, g_profile_id);
	}

	char external_path[MINI_PATH_SIZE] = {0};
	if (g_ext_files != NULL) {
		/* g_ext_files + /assets/ */
		snprintf(external_path, MINI_PATH_SIZE, "%s%s", g_ext_files, EXT_RESOURCES_SUBDIR);
	}

	char internal_path[MINI_PATH_SIZE] = {0};
	if (g_files_path != NULL) {
		/* g_files_path + /assets/ */
		snprintf(internal_path, MINI_PATH_SIZE, "%s%s", g_files_path, EXT_RESOURCES_SUBDIR);
	}

	const char *paths[] = {
		profile_external_path,
		profile_internal_path,
		external_path,
		internal_path,
	};

#define path_count (sizeof(paths)/sizeof(paths[0]))
	size_t lengths[path_count] = {0};

	/* sizeof global struct is sizeof header + all paths + NULL terminators */
	size_t size = sizeof(ResPaths);
	for (int i = 0; i < path_count; i++) {
		lengths[i] = strlen(paths[i]);
		size += (lengths[i] + 1);
	}

	ResPaths *new_block = realloc(global_res_paths, size);
	if (new_block == NULL) {
		/* Realistically there's no way to recover from this... */
		LOGE("Memory allocation error in Global Resource Paths!");
		return;
	}
	global_res_paths = new_block;

	size_t offset = 0;
	for (int i = 0; i < path_count; i++) {
		/* Skip non-existent entries. */
		if (lengths[i] == 0) {
			new_block->entries[i].path_len = 0;
			new_block->entries[i].path_pointer = NULL;
			continue;
		}

		new_block->entries[i].path_len = lengths[i];
		new_block->entries[i].path_pointer = new_block->path_block + offset;

		strcpy(new_block->path_block + offset, paths[i]);

		/* Offset must skip over NULL terminator. */
		offset += lengths[i] + 1;
	}
}

/** Given the name of a resource (without the `resources/` prefix), attempt to locate it.
 * Sets the Backend and Path in `out`.
 * In case of unknown resource, the path will be set to "" and the backend to NULL.
 * Since the input is an arg, it IS safe to use out->path as a temp var, I think. */
void miniR_locate_resource(ParsedPath *out, const char *_Nonnull path) {
	size_t out_size = sizeof(out->path);
	size_t input_len = strlen(path);

	if (input_len > out_size - 1) {
		/* There's no way this is going to fit. */
		goto path_length_exceeded;
	}

	if (global_res_paths == NULL) {
		LOGW("No search paths set for resources, only the APK will be checked!");
		goto check_apk;
	}

	/* Check all resource search paths. */
	for (size_t i = 0; i < RES_PATH_COUNT; i++) {
		ResPathEntry *entry = &global_res_paths->entries[i];

		/* Entry is either errored or not set, skip it. */
		if (entry->path_pointer == NULL) continue;

		const char *base_path = entry->path_pointer;
		size_t base_len = entry->path_len;

		if (base_len + input_len > out_size - 1) {
			LOGW("Length exceeded while locating resource '%s'", path);
			continue;
		}

		// LOGD("Trying to find resource in dir '%s' (%li)", base_path, base_len);

		/* Don't need to copy the NULL Terminator for the base. */
		memcpy(out->path, base_path, base_len);
		/* But do copy it here. */
		memcpy(out->path + base_len, path, input_len + 1);

		struct stat file_stat;
		/* Explicitly use the non-patched `stat` function. */
		if (stat(out->path, &file_stat) != 0)
			continue; /* Nonexistent */

		if (!(file_stat.st_mode & S_IRUSR)) {
			LOGW("Unreadable file '%s' in Resource directory '%s'", out->path, base_path);
			continue;
		}

		/* Found, readable. It's already in the out->path, just set backend. */
		out->backend = &backend_std;
		LOGD("Found resource externally: '%s' at '%s'", path, out->path);

		return;
	}
	check_apk:
	/* All File paths fell through, check Assets. */

	/* Don't need to copy the NULL Terminator for the base. */
	memcpy(out->path, APK_RESOURCES_SUBDIR, strlen(APK_RESOURCES_SUBDIR));
	/* But do copy it here. */
	memcpy(out->path + strlen(APK_RESOURCES_SUBDIR), path, input_len + 1);

	AAsset *asset = AAssetManager_open(g_asset_mgr, out->path, AASSET_MODE_STREAMING);
	if (asset != NULL) {
		/* Found asset! */
		LOGD("Found resource '%s' in APK", out->path);
		AAsset_close(asset);

		out->backend = &backend_assets;
		return;
	}
	/* Assets fell through too. Resource not found at all. */
	goto error;

	path_length_exceeded:
	LOGW("Path length exceeded in resource search: '%s'", path);
	error:
	out->path[0] = '\0';
	out->backend = NULL;
}
