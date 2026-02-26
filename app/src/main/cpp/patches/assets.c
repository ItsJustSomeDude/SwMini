#include "global.h"
#include "hooks.h"
#include "log.h"
#include "lua_libs/mini/mini.h"
#include "patches.h"
#include "features/mini_files/patched_functions.h"
#include "tools/files.h"
#include "features/mini_files/mini_files.h"
#include "features/mini_files/functions.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define LOG_TAG "MiniAssetsPatch"

/* These logs are very chatty, turn them off. */
#undef LOGD
#define LOGD(...) ((void)0)

#define VanillaResPrefix "resources/"


STATIC_DL_HOOK_SYMBOL(
	fileAtPath,
	"_ZN5Caver16FileExistsAtPathERKSs",
	bool, (char** assetPath)
) {
	const char *path = *assetPath;
	/*
	 * Given a path, check if a file exists and is readable at that path.
	 *
	 * It can either be a 'resources/', or a full absolute path. If it's absolute, just check for
	 * readability. If it's `resources/`, parse it as a Mini path, run `find_resource`, and return
	 * if it exists.
	 */

	bool is_resource = strncmp(path, VanillaResPrefix, strlen(VanillaResPrefix)) == 0;
	if (!is_resource) {
		/* Engine sent an absolute path, check it as a file. */
		struct stat fileStat;
		/* Explicitly use the non-patched `stat` function. */
		if ((stat) (path, &fileStat) != 0)
			return false; /* Nonexistent */

		if (!(fileStat.st_mode & S_IRUSR)) {
			LOGW("Engine queried nonexistent file '%s'", path);
			return false;
		}
		return true;
	}
	/* File starts with `resources/...`. Parse it as a Mini Path, then check if there's a real resource. */

	MiniPath p;
	parse_mini_path(&p, path);
	if (p.type != RESOURCE) {
		/* The engine tried to query for a non-resource file! That shouldn't happen... */
		LOGW("Engine queried non-resource file '%s'", path);
		return false;
	}

	locate_resource(&p);
	if (p.type == ERROR) {
		/* Does not exist. */
//		LOGW("Engine queried non-existent resource '%s'", p.path);
		return false;
	}

//	LOGD("Engine found resource '%s' at '%s'", path, p.path);
	return true;
}

STATIC_DL_HOOK_SYMBOL(
	openAsset,
	"_ZN5Caver29NewByteBufferFromAndroidAssetERKSsPj",
	void*,
	(char **asset_path, int* out_len)
) {
	const char *path = *asset_path;
	/* Given a resource path, opens resource and reads it into a buffer.
	 * Returns said buffer. Sets `out_len` to buffer size.
	 *
	 * Modified version should read path as MiniPath, and try it as a resource.
	 */

	MiniPath p;
	parse_mini_path(&p, path);
	if (p.type != RESOURCE) {
		LOGW("Engine tried loading non-resource file '%s'", path);
		goto error;
	}

	locate_resource(&p);
	if (p.type == ERROR) {
		LOGW("Engine tried loading non-existent resource '%s'", p.path);
		goto error;
	}

	MiniFILE *file = miniP_fopen(&p, "rb");
	if (file == NULL) {
		LOGE("Error while opening resource '%s'", path);
		goto error;
	}

	/* Move the file pointer to the end to determine the file size */
	miniP_fseek(file, 0, SEEK_END);
	size_t len = miniP_ftell(file);
	miniP_fseek(file, 0, SEEK_SET); /* Move back to the beginning of the file */

	void *buffer = malloc(len);
	if (buffer == NULL) {
		LOGE("Out of memory while loading resource '%s'", path);
		miniP_fclose(file);
		goto error;
	}

	/* TODO: Better to read in smaller increments? */
	size_t read_bytes = miniP_fread(buffer, 1, len, file);
	if (read_bytes != len) {
		LOGE("Failed to read asset '%s': expected %li, got %li", path, len, read_bytes);
		miniP_fclose(file);
		free(buffer);
		goto error;
	}

	char sz[32];
	format_bytes(sz, sizeof(sz), len);
	LOGD("%s Resource loaded from %s: '%s'", sz,
	     p.type == ASSET ? "assets" : "file", path);

	miniP_fclose(file);
	*out_len = (int) read_bytes;
	return buffer;

	error:
	*out_len = 0;
	return NULL;
}

void init_patch_assets() {
	LOGD("Applying External Assets patch");

	hook_openAsset();
	hook_fileAtPath();
}

/*
 * A handy list of vanilla functions that deal with Files directly:
 *
 * Must be able to handle `resources`:
 * bool Caver::FileExistsAtPath(const char **file_path);
 * Caver::GetAudioFileData(const char **file_path, Caver::AudioBuffer::BufferFormat*, void**, int*, int*);
 * void* Caver::NewByteBufferFromAndroidAsset(const char **path, unsigned int* out_len);
 * int Caver::OpenAAssetFileDescriptor(const char **path);
 *
 * Real files only:
 * Caver::CreateDirectoryAtPath(const char **dir_path, bool create_parents);
 * Caver::DeleteFileAtPath(const char **file_path);
 * void* Caver::NewByteBufferFromFile(char **file_path, unsigned int* out_len);
 * undefined4 Caver::SaveByteBufferToFile(unsigned char const*, unsigned int, std::string const&)
 * void Caver::GetFilesWithExtension(std::string const&, std::string const&, std::vector<std::string,
   std::allocator<std::string > >*)
 *
 * Unused?
 * Caver::LoadContentsOfFile(const char **file_path, std::string*)
 * */

//STATIC_DL_HOOK_SYMBOL(
//        gfwe,
//        "_ZN5Caver21GetFilesWithExtensionERKSsS1_PSt6vectorISsSaISsEE",
//        void*,
//        (void **param_1, char **path, void *vector)
//) {
//    // This lists all files in the folder `path` with the extension in param... uh... I'm not sure.
//    // I think they get stored to `vector`?
//    // I don't think this is ever used with Assets (no code seems to touch AAssetManager) so
//    // no patches should be needed.
//
//    // LOGD("FilesWithExtension: %p %p %p %ld %ld %ld %ld %ld", param_1, path, vector, param_4, param_5, param_6, param_7, param_8);
//    return orig_gfwe(param_1, path, vector, param_4, param_5, param_6, param_7, param_8);
//}
