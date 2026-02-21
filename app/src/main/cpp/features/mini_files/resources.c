#include <string.h>
#include <malloc.h>
#include "features/mini_files/patched_functions.h"
#include "log.h"
#include "global.h"
#include "mini_files.h"

#define LOG_TAG "MiniResources"

/** The global block of resource paths, all in contiguous memory for <i>SPEED!</i> */
ResourcePathBlock *g_resource_paths;

/* This should be in the same order as `g_resource_paths` */
const char *mini_prefixes[4] = {
	PrefixExtFiles,
	PrefixFiles,
	PrefixExtCache,
	PrefixCache,
};

/** Given a parsed Resource path (run through the Mini Path Parser, so it should NOT have the
 * 'resources' prefix), returns whether it should be found as a REAL path, or as an ASSET.
 *
 * The `out` var will be set to the path to find it at, either an AAsset path or a file path based
 * on the return type.
 *
 * Returns ERROR (-1) with `out` set to '' if the resource was not found at all. */
MiniPathType find_resource(char *out, size_t out_size, const char *input) {
	size_t input_len = strlen(input);
	if (input_len > out_size - 1) {
		/* There's no way this is going to fit. */
		goto path_length_exceeded;
	}

	if (g_resource_paths == NULL) {
		LOGW("No search paths set for resources, only the APK will be checked!");
		goto check_apk;
	}

	/* Check all resource search paths. */
	for (size_t i = 0; i < RES_PATH_COUNT; i++) {
		PathBlockEntry *entry = &g_resource_paths->entries[i];

		/* Entry is either errored or not set, skip it. */
		if (entry->path_len <= 0) continue;

		char *base_path = g_resource_paths->path_block + entry->path_offset;
		size_t base_len = entry->path_len;

		if (base_len + input_len > out_size - 1) {
			goto path_length_exceeded;
		}

		LOGD("Trying to find resource in dir '%s' (%li)", base_path, base_len);
		/* Don't need to copy the NULL Terminator for the base. */
		memcpy(out, base_path, base_len);
		/* But do copy it here. */
		memcpy(out + base_len, input, input_len + 1);

		struct stat fileStat;
		if (stat(out, &fileStat) != 0)
			continue; /* Nonexistent */

		if (!(fileStat.st_mode & S_IRUSR)) {
			LOGW("Unreadable file '%s' in Resource directory '%s'", input, base_path);
			continue;
		}

		/* Found, readable. It's already in `out`, so just return the type (file). */
		return REAL;
	}
	check_apk:
	/* All File paths fell through, check Assets. */

	/* Don't need to copy the NULL Terminator for the base. */
	memcpy(out, APK_RESOURCES_SUBDIR, strlen(APK_RESOURCES_SUBDIR));
	/* But do copy it here. */
	memcpy(out + strlen(APK_RESOURCES_SUBDIR), input, input_len + 1);

	AAsset *asset = AAssetManager_open(g_asset_mgr, out, AASSET_MODE_STREAMING);
	if (asset != NULL) {
		/* Found asset! Path is already set in `out`. */
		AAsset_close(asset);
		return ASSET;
	}
	/* Assets fell through too. Resource not found at all. */
	goto error;

	path_length_exceeded:
	LOGW("Path length exceeded in resource search: '%s'", input);
	error:
	*out = '\0';
	return ERROR;
}
