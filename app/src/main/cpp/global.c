#include "global.h"
#include "log.h"
#include "features/coin_limit.h"
#include "features/mini_files/mini_files.h"
#include "features/mini_files/patched_functions.h"

#include <android/asset_manager_jni.h>
#include <jni.h>
#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>

#define LOG_TAG "MiniGlobals"

AAssetManager *g_asset_mgr;

const char *g_ext_files = NULL;
const char *g_ext_cache = NULL;
const char *g_files_path = NULL;
const char *g_cache_path = NULL;

const char **g_path_list[4] = {
	&g_ext_files,
	&g_files_path,
	&g_ext_cache,
	&g_cache_path
};

const char *g_cur_profile_id;

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_LibraryManager_setMiniAssetManager(
	JNIEnv *env, jclass clazz, jobject manager) {
	jobject gbl = (*env)->NewGlobalRef(env, manager);
	g_asset_mgr = AAssetManager_fromJava(env, gbl);
}

const char *to_c_string(JNIEnv *env, jstring js) {
	const char *chars = (*env)->GetStringUTFChars(env, js, 0);
	const char *out = strdup(chars);
	(*env)->ReleaseStringUTFChars(env, js, chars);
	return out;
}

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_LibraryManager_setMiniFilePaths(
	JNIEnv *env, jclass clazz,
	jstring files, jstring cache,
	jstring ext_files, jstring ext_cache
) {
	g_files_path = to_c_string(env, files);
	g_cache_path = to_c_string(env, cache);
	g_ext_files = to_c_string(env, ext_files);
	g_ext_cache = to_c_string(env, ext_cache);

	set_global_resource_paths();

	char a[PATH_MAX];
	real_to_mini_path(a, PATH_MAX,
	                  "/data/user/0/net.itsjustsomedude.swrdg/files/something_cool.txt");
}

/* TODO: This should probably not live here... */
void miniG_set_profile_id(const char *id) {
	if (g_cur_profile_id != NULL) {
		// Free the old string copy.
		free((void *) g_cur_profile_id);
	}
	g_cur_profile_id = strdup(id);
	LOGD("Fetched new Profile ID: %s", g_cur_profile_id);

	/* Since we're entering a new Save file, reset the Coin Limit in case it was modified from Lua. */
	miniCL_reset();

	/* Update the Asset paths with the current profile id. */
	set_profile_resource_path(g_cur_profile_id);
}
