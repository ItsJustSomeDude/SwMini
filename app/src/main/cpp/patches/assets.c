#include <stdbool.h>
#include <sys/stat.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "../log.h"
#include "../hooks.h"
#include "../lua_ext/mini_files.h"
#include "../mini_lua/library.h"
#include "patches.h"

#define max(a, b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define LOG_TAG "MiniAssetsPatch"

#define ExtAssetsBase "assets/"
#define VanillaResPrefix "resources/"

bool pathReadable(char *path) {
	struct stat fileStat;
	if (stat(path, &fileStat) == 0) {
		// File Exists
		if (fileStat.st_mode & S_IRUSR) {
			// Readable
			return true;
		}
	}
	return false;
}

void *bufferFromPath(char *path, int *length) {
	FILE *file = fopen(path, "rb"); // Open the file in binary mode
	if (!file) {
		perror("Failed to open file");
		return NULL;
	}

	// Move the file pointer to the end to determine the file size
	fseek(file, 0, SEEK_END);
	int fileSize = ftell(file);
	fseek(file, 0, SEEK_SET); // Move back to the beginning of the file

	// Store the file length to the passed pointer.
	*length = fileSize;

	// Allocate memory for the buffer
	unsigned char *buffer = malloc(fileSize);
	if (buffer == NULL) {
		perror("Failed to allocate memory");
		fclose(file);
		return NULL;
	}

	// Read the file content into the buffer
	size_t bytesRead = fread(buffer, 1, fileSize, file);
	if (bytesRead != fileSize) {
		perror("Failed to read the complete file");
		free(buffer);
		fclose(file);
		return NULL;
	}

	LOGD("Asset %s: loaded with length %d", path, fileSize);

	return buffer;
}

void *tryBufferFromPath(char *path, int *length) {
	if (pathReadable(path)) {
		LOGD("AssetTry %s: Loaded", path);
		return bufferFromPath(path, length);
	}
	return NULL;
}

//STATIC_DL_HOOK_SYMBOL(tst1, "_ZN5Caver19IsAndroidAssetsPathERKSs", bool, (char** path)) {
//    // This appears to do a basic String comparison, and if it starts with resources/ then in returns true.
//    // I don't think this requires any patches, at least for now?
//
//    bool ret = orig_tst1(path);
//    LOGD("'%s' asset: %s", *path, ret ? "true" : "false");
//    return ret;
//}

//STATIC_DL_HOOK_SYMBOL(
//        gfwe,
//        "_ZN5Caver21GetFilesWithExtensionERKSsS1_PSt6vectorISsSaISsEE",
//        void*,
//        (void **param_1, char **path, void *vector, long param_4,
//                long param_5, long param_6, long param_7, long param_8)
//) {
//    // This lists all files in the folder `path` with the extension in param... uh... I'm not sure.
//    // I think they get stored to `vector`?
//    // I don't think this is ever used with Assets (no code seems to touch AAssetManager) so
//    // no patches should be needed.
//
//    // LOGD("FilesWithExtension: %p %p %p %ld %ld %ld %ld %ld", param_1, path, vector, param_4, param_5, param_6, param_7, param_8);
//    return orig_gfwe(param_1, path, vector, param_4, param_5, param_6, param_7, param_8);
//}


STATIC_DL_HOOK_SYMBOL(fileAtPath, "_ZN5Caver16FileExistsAtPathERKSs", bool, (char** assetPath)) {
	// This checks if a file exists, and _maybe_ if it's readable.
	// If it starts with `resources/` it tries to open it from the Asset Manager.
	// If that succeeds it closes the asset and returns true.
	// If it's not a "resource", it uses `stat` and returns success if that works.

	// Patched version:
	// If it starts with "resources/", try the 4 external paths.
	// If that fails, fall back to the original.
	// Also use the original if it's not a resource.

	bool isResource = strncmp(*assetPath, VanillaResPrefix, strlen(VanillaResPrefix)) == 0;
	LOGD("FileExists: %s is resource? %d", *assetPath, isResource);

	if (!isResource) return orig_fileAtPath(assetPath);

	// Try all 4 externals...

	bool validProfile = latestProfileId != NULL;
	size_t profileIdLen = validProfile ? strlen(latestProfileId) : 0;

	long maxPathLen =
		// Longest base path
		max(strlen(externalFilesPath), strlen(filesPath))
		+ strlen(*assetPath)
		+ profileIdLen
		// Length of static path segment(s)
		+ strlen(ExtAssetsBase)
		// One extra for Terminator, One extra for added /, One extra because I probably forgot something.
		+ 3;
	char *absFilePath = malloc(maxPathLen);
	if (absFilePath == NULL) {
		LOGE("Out of memory for patched asset checking!");
		return NULL;
	}

	if (validProfile && externalFilesPath != NULL) {
		sprintf(absFilePath, "%s%s%s/%s", externalFilesPath, ExtAssetsBase, latestProfileId,
		        *assetPath);
		if (pathReadable(absFilePath)) {
			free(absFilePath);
			return true;
		}
	}

	if (filesPath != NULL) {
		sprintf(absFilePath, "%s%s%s/%s", filesPath, ExtAssetsBase, latestProfileId, *assetPath);
		if (pathReadable(absFilePath)) {
			free(absFilePath);
			return true;
		}
	}

	if (externalFilesPath != NULL) {
		sprintf(absFilePath, "%s%s%s", externalFilesPath, ExtAssetsBase, *assetPath);
		if (pathReadable(absFilePath)) {
			free(absFilePath);
			return true;
		}
	}

	if (filesPath != NULL) {
		sprintf(absFilePath, "%s%s%s", filesPath, ExtAssetsBase, *assetPath);
		if (pathReadable(absFilePath)) {
			free(absFilePath);
			return true;
		}
	}

	// Fall back to original for Assets check.
	return orig_fileAtPath(assetPath);
}

STATIC_DL_HOOK_SYMBOL(
	openAsset,
	"_ZN5Caver29NewByteBufferFromAndroidAssetERKSsPj",
	void*,
	(char **assetPath, int* outLength)
) {
	// So... This takes a path and a pointer where the Length will be stored. (I think).
	// It opens the asset at the path, fetches the length, creates a buffer of that size, and reads the asset into it.
	// The asset is then closed.
	// Then it returns a pointer to the start of that new buffer.

	// Modified version:
	// (resources/... is already part of the path.)
	// If <latestProfile> is not null, try to open from /ExternalFiles/assets/<latestProfile>/<path>
	// If <latestProfile> is not null, try to open from /Files/assets/<latestProfile>/<path>
	// Try to open from /ExternalFiles/assets/<path>
	// Try to open from /Files/assets/<path>
	// Failing that, use Original function.

	bool validProfile = latestProfileId != NULL;
	size_t profileIdLen = validProfile ? strlen(latestProfileId) : 0;

	long maxPathLen =
		// Longest base path
		max(strlen(externalFilesPath), strlen(filesPath))
		+ strlen(*assetPath)
		+ profileIdLen
		// Length of static path segment(s)
		+ sizeof(ExtAssetsBase)
		// One extra for Terminator, One extra for added /, One extra because I probably forgot something.
		+ 3;
	char *absFilePath = malloc(maxPathLen);
	if (absFilePath == NULL) {
		LOGE("Out of memory for patched asset loading!");
		return NULL;
	}

	if (validProfile && externalFilesPath != NULL) {
		sprintf(absFilePath, "%s%s%s/%s", externalFilesPath, ExtAssetsBase, latestProfileId,
		        *assetPath);
		void *buf = tryBufferFromPath(absFilePath, outLength);
		if (buf != NULL) {
			free(absFilePath);
			return buf;
		}
	}

	if (validProfile && filesPath != NULL) {
		sprintf(absFilePath, "%s%s%s/%s", filesPath, ExtAssetsBase, latestProfileId, *assetPath);
		void *buf = tryBufferFromPath(absFilePath, outLength);
		if (buf != NULL) {
			free(absFilePath);
			return buf;
		}
	}

	if (externalFilesPath != NULL) {
		sprintf(absFilePath, "%s%s%s", externalFilesPath, ExtAssetsBase, *assetPath);
		void *buf = tryBufferFromPath(absFilePath, outLength);
		if (buf != NULL) {
			free(absFilePath);
			return buf;
		}
	}

	if (filesPath != NULL) {
		sprintf(absFilePath, "%s%s%s", filesPath, ExtAssetsBase, *assetPath);
		void *buf = tryBufferFromPath(absFilePath, outLength);
		if (buf != NULL) {
			free(absFilePath);
			return buf;
		}
	}

	void *ret = orig_openAsset(assetPath, outLength);
	LOGD("%s: Using APK asset, length %d", *assetPath, *outLength);
	return ret;
}

void setupAssetPatch() {
	LOGD("Applying External Assets patch");

	hook_openAsset();
	hook_fileAtPath();
}
