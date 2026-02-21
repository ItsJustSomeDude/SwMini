#include "patched_functions.h"

#include "global.h"
#include "log.h"
#include "lua_libs/mini/mini.h"
#include "tools/files.h"
#include "mini_files.h"

#include <bits/strcasecmp.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>
#include <stdlib.h>
#include <errno.h>

struct utimbuf;

#define LOG_TAG "MiniLuaPaths"

/* Special Prefixes:
 *
 * Always load from disk:
 * /Files/...
 * /ExternalFiles/...
 * /Cache/...
 * /ExternalCache/...
 *
 * Always load from AAssetManager:
 * /Assets/...
 *
 *
 * /../ is never allowed in the input path!
 * /./ will be silently removed.
 *
 * Inputs:
 *
 * -> App Data Directories (with optional mod name suffix.)
 *
 * "/Assets/..." -> Uses AAssetManager, searches from the root of the `/assets/` directory in the APK.
 * Used by `require` rocks loader.
 *
 * "resources/..." -> Progressive Search path:
 * `/ExtFiles/assets/resources/<profile>/`,
 * `/Files/assets/resources/<profile>/`,
 * `/ExtFiles/assets/resources/`,
 * `/Files/assets/resources/`,
 * `/Assets/resources/`
 *
 * If any of the path segments are "/./", remove them.
 * If any of the path segments are "/../", then... fail? To disallow path traversal exploits?
 */

bool unreadable(MiniPath *p) {
	if (p->type == ERROR || p->type == UNKNOWN) {
		errno = ENOENT; /* No such file or directory. */
		return true;
	}
	return false;
}

bool unwritable(MiniPath *p) {
	if (p->type == ERROR || p->type == UNKNOWN) {
		errno = ENOENT; /* No such file or directory. */
		return true;
	} else if (p->type == ASSET || p->type == RESOURCE) {
		errno = EROFS; /* Read-only FS, because assets and resources cannot be written. */
		return true;
	}
	return false;
}

bool unreal(MiniPath *p) {
	if (p->type == REAL) return false;
	errno = EACCES;
	return true;
}

int mini_path_symlink(const char *_Nonnull old_path, const char *_Nonnull new_path) {
	MiniPath o1;
	MiniPath o2;
	parse_mini_path(&o1, old_path);
	parse_mini_path(&o2, new_path);

	if (unreadable(&o1) || unwritable(&o2)) return -1;
	if (o1.type != REAL) {
		errno = EXDEV; /* Cross-Device Error. Only real file can be source of link. */
		return -1;
	}
	return symlink(o1.path, o2.path);
}

int mini_path_link(const char *_Nonnull old_path, const char *_Nonnull new_path) {
	MiniPath o1;
	MiniPath o2;
	parse_mini_path(&o1, old_path);
	parse_mini_path(&o2, new_path);

	if (unreadable(&o1) || unwritable(&o2)) return -1;
	if (o1.type != REAL) {
		errno = EXDEV; /* Cross-Device Error. Only real file can be source of link. */
		return -1;
	}
	return link(o1.path, o2.path);
}

int mini_path_unlink(const char *_Nonnull path) {
	MiniPath p;
	parse_mini_path(&p, path);
	if (unreadable(&p) || unwritable(&p)) return -1;

	return unlink(p.path);
}

int mini_path_mkdir(const char *_Nonnull path, mode_t mode) {
	MiniPath p; /* Real FS path will be parsed into `p.path`. */
	parse_mini_path(&p, path);
	if (unreadable(&p) || unwritable(&p)) return -1;

	return mkdir(p.path, mode);
}

int mini_path_mkdir_p(const char *_Nonnull path, mode_t mode) {
	MiniPath p; /* p.path is real FS path. */
	parse_mini_path(&p, path);
	if (unreadable(&p) || unwritable(&p)) return -1;

	return mkdir_p(p.path, mode);
}

int mini_path_rmdir(const char *_Nonnull path) {
	MiniPath p;
	parse_mini_path(&p, path);
	if (unreadable(&p) || unwritable(&p)) return -1;

	return rmdir(p.path);
}

DIR *_Nullable mini_path_opendir(const char *_Nonnull path) {
	MiniPath p;
	parse_mini_path(&p, path);
	if (unreadable(&p)) return NULL;

	return opendir(p.path);
}

struct dirent *_Nullable mini_path_readdir(DIR *_Nonnull dir) {
	struct dirent *dir_entry = readdir(dir);
	if (!dir_entry) return NULL;

	/* TODO: This probably has lots of problems... */

	const size_t out_size = sizeof(dir_entry->d_name);
	char out[out_size];
	real_to_mini_path(out, sizeof(out), dir_entry->d_name);

	LOGD("Mini Readdir says: %p %s -> %s", dir_entry->d_name, dir_entry->d_name, out);
	strncpy(dir_entry->d_name, out, out_size);

	return dir_entry;
}

ssize_t mini_path_readlink(const char *_Nonnull path, char *_Nonnull buf, size_t buf_size) {
	MiniPath p;
	parse_mini_path(&p, path);
	if (unreadable(&p)) return -1;

	ssize_t bytes_written = readlink(p.path, buf, buf_size);
	if (bytes_written < 0)
		return -1; /* readlink errored. */

	if (bytes_written == buf_size)
		return bytes_written; /* Possibly truncated. */

	/* If we get here, there's at least 1 free byte in buffer. */
	buf[bytes_written + 1] = '\0';



	// TODO: Change Name of returned thing.
	// This is going to be hard, because there's not a NULL byte.
	LOGD("ReadLink did thing. Cannot print though...");

	return bytes_written;
}

int mini_path_utime(const char *_Nonnull filename, const struct utimbuf *_Nullable times) {
	MiniPath p;
	parse_mini_path(&p, filename);
	if (unreadable(&p) || unwritable(&p)) return -1;

	return utime(p.path, times);
}

FILE *_Nullable mini_path_fopen(const char *_Nonnull path, const char *_Nonnull mode) {
	MiniPath p;
	parse_mini_path(&p, path);
	if (unreadable(&p)) return NULL;

	if (mode[0] != '\0') {
		/* Check if file is trying to be opened in any Write or Append mode. */
		if (mode[0] == 'w' || mode[0] == 'a' || (mode[0] == 'r' && mode[1] == '+')) {
			if (unwritable(&p)) return NULL;
		}
	}

	return fopen(p.path, mode);
}

int mini_path_remove(const char *_Nonnull path) {
	MiniPath p;
	parse_mini_path(&p, path);
	if (unreadable(&p) || unwritable(&p)) return -1;

	return remove(p.path);
}

int mini_path_rename(const char *_Nonnull old_path, const char *_Nonnull new_path) {
	MiniPath o1;
	MiniPath o2;
	parse_mini_path(&o1, old_path);
	parse_mini_path(&o2, new_path);

	if (unreadable(&o1) || unwritable(&o1) || unwritable(&o2)) return -1;
	return rename(o1.path, o2.path);
}

int mini_path_lstat(const char *_Nonnull path, struct stat *_Nonnull buf) {
	MiniPath p;
	parse_mini_path(&p, path);
	if (unreadable(&p)) return -1;

	return remove(p.path);

//	char out[PATH_MAX];
//	return lstat(from_mini_path(out, sizeof(out), path), buf);
}

int mini_path_stat(const char *_Nonnull path, struct stat *_Nonnull buf) {
//	LOGD("Did a stat thing");
//	char out[PATH_MAX];
//	return stat(from_mini_path(out, sizeof(out), path), buf);
}
