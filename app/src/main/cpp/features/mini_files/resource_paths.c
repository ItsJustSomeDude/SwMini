#include "mini_files.h"

#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>
#include "log.h"
#include "global.h"

#define LOG_TAG "MiniResourcePaths"

void set_resource_path(GlobalResourcePath a, const char *path) {
	size_t path_len = strlen(path);

	/* We have the Path and its length. We need to allocate a new global structure to store
	 * _everything_, put the existing paths and data into it, and insert the new one.
	 *
	 * Before allocating, we need to calculate the size...
	 *
	 * We need to find out how big the global structure needs to be,
	 * reallocate it, */

	size_t size = sizeof(ResourcePathBlock);
	if (g_resource_paths != NULL) {
		/* There are already some strings set, read the existing lengths. */
		for (size_t i = 0; i < RES_PATH_COUNT; i++) {
			/* Skip the entry we are setting, and entries with no length. */
			if (i == a || g_resource_paths->entries[i].path_len <= 0) continue;

			size += (g_resource_paths->entries[i].path_len + 1);
		}
	}
	size += path_len + 1;

//	LOGD("New Resource Path block length: %li. String is %li, header is %li",
//	     size, path_len, sizeof(ResourcePathBlock));

	ResourcePathBlock *old_block = g_resource_paths;
	ResourcePathBlock *new_block = calloc(1, size);
	if (new_block == NULL) {
		LOGE("Out of memory while updating Resource search paths!");
		return;
	}

	size_t string_write_offset = 0;
	/* There are already some strings set, read the existing lengths. */
	for (size_t i = 0; i < RES_PATH_COUNT; i++) {
		PathBlockEntry *new_entry = &new_block->entries[i];

		if (i == a) {
			/* Current block */

			new_entry->path_len = path_len;
			new_entry->path_offset = string_write_offset;

			memcpy(
				new_block->path_block + string_write_offset,
				path, path_len + 1
			);
			string_write_offset += path_len + 1;

//			LOGD("Copied current string '%s' into block.",
//			     new_block->path_block + new_entry->path_offset);

		} else if (old_block != NULL && old_block->entries[i].path_len > 0) {
			PathBlockEntry *old_entry = &g_resource_paths->entries[i];

			/* Copy info into new block from old. */
			new_entry->path_len = old_entry->path_len;
			new_entry->path_offset = string_write_offset;

			/* Copy string from old block into new. Advance write location for next string. */
			memcpy(
				new_block->path_block + string_write_offset,
				g_resource_paths->path_block + g_resource_paths->entries[i].path_offset,
				g_resource_paths->entries[i].path_len + 1
			);
			string_write_offset += g_resource_paths->entries[i].path_len + 1;

//			LOGD("Copied old string '%s' into block.",
//			     new_block->path_block + new_entry->path_offset);
		}
		/* Else, the old block did not have an entry for this, so just skip it to keep the output packed. */
	}
	/* Old and new stuff written! */

	free(old_block);
	g_resource_paths = new_block;

	LOGD("Updated resource Search path: %i -> '%s'", a, path);
}

void set_profile_resource_path(const char *profile_id) {
	char path[PATH_MAX];

	if (g_ext_files != NULL) {
		/* g_ext_files + /assets/ + profile_id + / */
		snprintf(path, PATH_MAX, "%s%s%s/",
		         g_ext_files, EXT_RESOURCES_SUBDIR, profile_id);

		set_resource_path(PROFILE_EXTERNAL, path);
	}

	if (g_files_path != NULL) {
		/* g_files_path + /assets/ + profile_id + / */
		snprintf(path, PATH_MAX, "%s%s%s/",
		         g_files_path, EXT_RESOURCES_SUBDIR, profile_id);

		set_resource_path(PROFILE_INTERNAL, path);
	}
}

void set_global_resource_paths() {
	char path[PATH_MAX];

	if (g_ext_files != NULL) {
		/* g_ext_files + /assets/ */
		snprintf(path, PATH_MAX, "%s%s", g_ext_files, EXT_RESOURCES_SUBDIR);

		set_resource_path(GLOBAL_EXTERNAL, path);
	}

	if (g_files_path != NULL) {
		/* g_files_path + /assets/ */
		snprintf(path, PATH_MAX, "%s%s", g_files_path, EXT_RESOURCES_SUBDIR);

		set_resource_path(GLOBAL_INTERNAL, path);
	}
}
