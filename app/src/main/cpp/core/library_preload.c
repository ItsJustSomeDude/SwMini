#include <jni.h>
#include <dlfcn.h>
#include "log.h"
#include "hooks.h"
#include "init/core.h"

#define LOG_TAG "MiniLibPreload"

JNIEXPORT jboolean JNICALL
Java_net_itsjustsomedude_swrdg_LibraryManager_nativePreloadEngine(
	JNIEnv *env, jclass clazz, jstring jLibPath
) {
	const char *lib_path = (*env)->GetStringUTFChars(env, jLibPath, 0);
	if (lib_path == NULL) {
		(*env)->ReleaseStringUTFChars(env, jLibPath, lib_path);
		LOGE("Failed to preload library: JNI Internal Error.");
		return false;
	}

	LOGD("Performing engine preload at path %s", lib_path);

	engine_dl_handle = dlopen(lib_path, RTLD_NOW);
	if (engine_dl_handle == NULL) {
		LOGE("Failed to preload library: %s", dlerror());
		/* Technically, we should call this after dlerror, even though they should not affect each other. */
		(*env)->ReleaseStringUTFChars(env, jLibPath, lib_path);
		return false;
	}

	(*env)->ReleaseStringUTFChars(env, jLibPath, lib_path);

	return true;
}

/* Almost the same as above, but doesn't set the engine handle.
 * This is used to load OpenAL. */
JNIEXPORT jboolean JNICALL
Java_net_itsjustsomedude_swrdg_LibraryManager_nativePreloadLibrary(
	JNIEnv *env, jclass clazz, jstring jLibPath
) {
	const char *lib_path = (*env)->GetStringUTFChars(env, jLibPath, 0);
	if (lib_path == NULL) {
		(*env)->ReleaseStringUTFChars(env, jLibPath, lib_path);
		LOGE("Failed to preload library: JNI Internal Error.");
		return false;
	}

	LOGD("Performing library preload at path %s", lib_path);

	void *handle = dlopen(lib_path, RTLD_NOW);
	if (handle == NULL) {
		LOGE("Failed to preload library: %s", dlerror());
		/* Technically, we should call this after dlerror, even though they should not affect each other. */
		(*env)->ReleaseStringUTFChars(env, jLibPath, lib_path);
		return false;
	}

	(*env)->ReleaseStringUTFChars(env, jLibPath, lib_path);

	return true;
}