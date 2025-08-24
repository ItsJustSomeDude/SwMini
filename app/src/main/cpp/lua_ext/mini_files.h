#ifndef SWMINI_MINI_H
#define SWMINI_MINI_H

#include <jni.h>

extern const char* externalFilesPath;
extern const char* externalCachePath;
extern const char* filesPath;
extern const char* cachePath;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_NativeBridge_setPaths(
        JNIEnv *env, jclass clazz,
        jstring files, jstring cache, jstring ext_files, jstring ext_cache);

const char* convertPath(const char* path);

#ifdef __cplusplus
}
#endif

#endif //SWMINI_MINI_H
