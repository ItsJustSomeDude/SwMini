#include <jni.h>
#include <android/asset_manager_jni.h>
#include "assets.h"

AAssetManager *g_asset_mgr;

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_LibraryManager_setMiniAssetManager(
	JNIEnv *env, jclass clazz, jobject manager
) {
	jobject gbl = (*env)->NewGlobalRef(env, manager);
	g_asset_mgr = AAssetManager_fromJava(env, gbl);
}
