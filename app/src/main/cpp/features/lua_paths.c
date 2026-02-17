#include <stddef.h>
#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <linux/limits.h>
#include "log.h"
#include "lua_paths.h"

#define LOG_TAG "MiniLuaPaths"

const char *externalFilesPath = NULL;
const char *externalCachePath = NULL;
const char *filesPath = NULL;
const char *cachePath = NULL;

const char *to_c_string(JNIEnv *env, jstring js) {
	const char *chars = (*env)->GetStringUTFChars(env, js, 0);
	const char *out = strdup(chars);
	(*env)->ReleaseStringUTFChars(env, js, chars);
	return out;
}

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_NativeBridge_setPaths(
	JNIEnv *env, jclass clazz,
	jstring files, jstring cache, jstring ext_files, jstring ext_cache) {

	filesPath = to_c_string(env, files);
	cachePath = to_c_string(env, cache);
	externalFilesPath = to_c_string(env, ext_files);
	externalCachePath = to_c_string(env, ext_cache);
}

#define string_pfx_eq_case(s1, s2) (strncasecmp(s1, s2, strlen(s2)) == 0)

const char *convert_path(
	char *output, size_t out_size,
	const char *input,
	const char *old_prefix,
	const char *new_prefix
) {
	// TODO: Terminate the buffer if it exceeds the length of the out buffer.
	snprintf(output, out_size, "%s%s", new_prefix, input + strlen(old_prefix));
	LOGD("Translated Lua path: '%s' -> '%s'", input, output);
	return output;
}

const char *parse_mini_path(char *output, size_t out_size, const char *input) {
	// Inputs:

	// "/{Files,ExternalFiles,Cache,ExternalCache}/..." -> App Data Directories (with optional mod name suffix.)

	// "/Assets/..." -> Uses AAssetManager, searches from the root of the `/assets/` directory in the APK.
	// Used by `require` rocks loader.

	// "resources/..." -> Progressive Search path:
	// `/ExtFiles/assets/resources/<profile>/`,
	// `/Files/assets/resources/<profile>/`,
	// `/ExtFiles/assets/resources/`,
	// `/Files/assets/resources/`,
	// `/Assets/resources/`



	// If any of the path segments are "/./", remove them.
	// If any of the path segments are "/../", then... fail? To disallow path traversal exploits?

	// Instant fail if input is NULL or empty string
	if (input == NULL || *input == '\0')
		return NULL;

	size_t write_pos = 0;
	size_t read_pos = 0;

	size_t cur_seg_start = 0;
	size_t cur_seg_length = 0;

//	while

	return NULL;
}

const char *from_mini_path(char *out, size_t out_size, const char *path) {
	if (externalFilesPath != NULL && string_pfx_eq_case(path, EXTERNAL_FILES_BASE)) {
		return convert_path(out, out_size, path, EXTERNAL_FILES_BASE, externalFilesPath);

	} else if (filesPath != NULL && string_pfx_eq_case(path, FILES_BASE)) {
		return convert_path(out, out_size, path, FILES_BASE, filesPath);

	} else if (cachePath != NULL && string_pfx_eq_case(path, CACHE_BASE)) {
		return convert_path(out, out_size, path, CACHE_BASE, cachePath);

	} else if (externalCachePath != NULL && string_pfx_eq_case(path, EXTERNAL_CACHE_BASE)) {
		return convert_path(out, out_size, path, EXTERNAL_CACHE_BASE, externalCachePath);
	}

	LOGW("Attempted to use invalid MiniPath '%s'", path);
	*out = '\0';
	return out;
}

const char *to_mini_path(char *out, size_t out_size, const char *path) {
	if (externalFilesPath != NULL && string_pfx_eq_case(path, externalFilesPath)) {
		return convert_path(out, out_size, path, externalFilesPath, EXTERNAL_FILES_BASE);

	} else if (filesPath != NULL && string_pfx_eq_case(path, filesPath)) {
		return convert_path(out, out_size, path, filesPath, FILES_BASE);

	} else if (cachePath != NULL && string_pfx_eq_case(path, cachePath)) {
		return convert_path(out, out_size, path, cachePath, CACHE_BASE);

	} else if (externalCachePath != NULL && string_pfx_eq_case(path, externalCachePath)) {
		return convert_path(out, out_size, path, externalCachePath, EXTERNAL_CACHE_BASE);
	}

	LOGW("Attempted to convert invalid path '%s'", path);
	*out = '\0';
	return out;
}

int mini_path_symlink(const char *_Nonnull old_path, const char *_Nonnull new_path) {
	char out1[PATH_MAX];
	char out2[PATH_MAX];
	return symlink(
		from_mini_path(out1, sizeof(out1), old_path),
		from_mini_path(out2, sizeof(out2), new_path)
	);
}

int mini_path_link(const char *_Nonnull old_path, const char *_Nonnull new_path) {
	char out1[PATH_MAX];
	char out2[PATH_MAX];
	return link(
		from_mini_path(out1, sizeof(out1), old_path),
		from_mini_path(out2, sizeof(out2), new_path)
	);
}

int mini_path_unlink(const char *_Nonnull path) {
	char out[PATH_MAX];
	return unlink(from_mini_path(out, sizeof(out), path));
}

int mini_path_mkdir(const char *_Nonnull path, mode_t mode) {
	char out[PATH_MAX];
	return mkdir(from_mini_path(out, sizeof(out), path), mode);
}

int mini_path_rmdir(const char *_Nonnull path) {
	char out[PATH_MAX];
	return rmdir(from_mini_path(out, sizeof(out), path));
}

DIR *_Nullable mini_path_opendir(const char *_Nonnull path) {
	char out[PATH_MAX];
	return opendir(from_mini_path(out, sizeof(out), path));
}

struct dirent *_Nullable mini_path_readdir(DIR *_Nonnull dir) {
	struct dirent *dir_entry = readdir(dir);
	if (!dir_entry) return NULL;

	char out[sizeof(dir_entry->d_name)];
	const char *converted = to_mini_path(out, sizeof(out), dir_entry->d_name);

	LOGD("Mini Readdir says: %p %s -> %s", dir_entry->d_name, dir_entry->d_name, converted);

	// strlen+1 to include the NULL byte.
	memcpy(dir_entry->d_name, converted, strlen(converted) + 1);

	return dir_entry;
}

ssize_t mini_path_readlink(const char *_Nonnull path, char *_Nonnull buf, size_t buf_size) {
	char mini_path[PATH_MAX];
	ssize_t len = readlink(from_mini_path(mini_path, sizeof(mini_path), path), buf, buf_size);
	if (len < 0) {
		return -1;
	}

//	char out[buf_size];
//	to_mini_path(out, sizeof(out), buf);

	// TODO: Change Name of returned thing.
	// This is going to be hard, because there's not a NULL byte.
	LOGD("ReadLink did thing. Cannot print though...");

	return len;
}

int mini_path_utime(const char *_Nonnull filename, const struct utimbuf *_Nullable times) {
	char out[PATH_MAX];
	return utime(from_mini_path(out, sizeof(out), filename), times);
}

FILE *_Nullable mini_path_fopen(const char *_Nonnull path, const char *_Nonnull mode) {
	char out[PATH_MAX];
	return fopen(from_mini_path(out, sizeof(out), path), mode);
}

int mini_path_remove(const char *_Nonnull path) {
	char out[PATH_MAX];
	return remove(from_mini_path(out, sizeof(out), path));
}

int mini_path_rename(const char *_Nonnull old_path, const char *_Nonnull new_path) {
	char out1[PATH_MAX];
	char out2[PATH_MAX];
	return rename(
		from_mini_path(out1, sizeof(out1), old_path),
		from_mini_path(out2, sizeof(out2), new_path)
	);
}

int mini_path_lstat(const char *_Nonnull path, struct stat *_Nonnull buf) {
	char out[PATH_MAX];
	return lstat(from_mini_path(out, sizeof(out), path), buf);
}

int mini_path_stat(const char *_Nonnull path, struct stat *_Nonnull buf) {
	LOGD("Did a stat thing");
	char out[PATH_MAX];
	return stat(from_mini_path(out, sizeof(out), path), buf);
}
