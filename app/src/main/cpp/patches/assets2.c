#include "patches.h"
#include "libs/khash.h"
#include "patches/assets/assets.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>

//AAssetManager *asset_manager;

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_NativeBridge_setMiniAssetManager(
	JNIEnv *env, jclass clazz, jobject manager
) {
	jobject gbl = (*env)->NewGlobalRef(env, manager);
	asset_manager = AAssetManager_fromJava(env, gbl);
}


void init_patch_assets2() {

}
