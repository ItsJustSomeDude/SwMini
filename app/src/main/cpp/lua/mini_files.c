
#include <stddef.h>
#include "mini_files.h"

#include <string.h>
#include <stdio.h>
#include <linux/limits.h>
#include <jni.h>

const char* externalFilesPath = NULL;
const char* externalCachePath = NULL;
const char* filesPath = NULL;
const char* cachePath = NULL;

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_NativeBridge_setPaths(
        JNIEnv *env, jclass clazz,
        jstring files, jstring cache, jstring ext_files, jstring ext_cache)
{
#define setPath(l, js, cs) \
    const char* l = (*env)->GetStringUTFChars(env,js,0); \
    cs = strdup(l); \
    (*env)->ReleaseStringUTFChars(env, js, l);

    setPath(f, files, filesPath)
    setPath(c, cache, cachePath)
    setPath(ef, ext_files, externalFilesPath)
    setPath(ec, ext_cache, externalCachePath)
}


#define EXTERNAL_FILES_BASE "/ExternalFiles/"
#define FILES_BASE "/Files/"
#define CACHE_BASE "/Cache/"
#define EXTERNAL_CACHE_BASE "/ExternalCache/"

const char* convertPath(const char* path) {

#define CMP(PATH, REPLACE) \
    if (REPLACE != NULL && strncasecmp(path, PATH, strlen(PATH)) == 0) { \
        static char result[PATH_MAX];                               \
        snprintf(result, PATH_MAX, REPLACE, path + strlen(PATH));   \
        printf("Translated Path: %s\n", result);                      \
        return result;                                              \
    }

    CMP(EXTERNAL_FILES_BASE, externalFilesPath)
    else CMP(FILES_BASE, filesPath)
    else CMP(CACHE_BASE, cachePath)
    else CMP(EXTERNAL_CACHE_BASE, externalCachePath)

    return "";
}
