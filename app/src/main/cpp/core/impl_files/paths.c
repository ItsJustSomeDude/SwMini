#include "paths.h"

#include <jni.h>
#include <string.h>

#define LOG_TAG "MiniCoreResources"

const char *g_ext_files = NULL;
const char *g_ext_cache = NULL;
const char *g_files_path = NULL;
const char *g_cache_path = NULL;

static const char *to_c_string(JNIEnv *env, jstring js) {
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
}
