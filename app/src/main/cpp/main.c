#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <android/log.h>

#include "lni.h"
#include "hook.h"
#include "libs/Gloss.h"

static JavaVM* g_vm = NULL;
static jobject g_bridgeClass = NULL;

// Early Loading - Executed before LibSwordigo is loaded into memory.
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    __android_log_write(ANDROID_LOG_DEBUG, "MiniNative", "Mini Natives Loaded! Attempting to start LNI thread...");
    g_vm = vm;

    spawn_lni_thread();

    GlossInit(false);
    GlossEnableLog(true);

    return JNI_VERSION_1_6;
}

void *lni_thread(void *_) {
    JNIEnv *env;
    if ((*g_vm)->AttachCurrentThread(g_vm, (JNIEnv**) &env, NULL) != 0) {
        __android_log_write(ANDROID_LOG_FATAL, "MiniNative", "Failed to attach LNI thread to JVM");
        return NULL;
    }

    process_lni_pipe(); // Long-running process that reads a pipe and calls send_lni_buffer when appropriate.
    (*g_vm)->DetachCurrentThread(g_vm);
    return NULL;
}

void send_lni_buffer(char *buffer) {
    __android_log_write(ANDROID_LOG_DEBUG, "LNI", buffer);

    if(buffer[0] == '\xFF' || buffer[0] == '\x7F') {
        // printf("Fixing byte: %c", buffer[0]);
        buffer[0] = '+';
    }

    JNIEnv *env;
    if ((*g_vm)->GetEnv(g_vm, (void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        __android_log_write(ANDROID_LOG_FATAL, "MiniNative", "Native Thread not attached (this should not happen!)");
        return;
    }

    jclass clazz = (jclass)g_bridgeClass;

    jmethodID method = (*env)->GetStaticMethodID(env, clazz, "processLni", "(Ljava/lang/String;)V");
    if (!method) {
        __android_log_write(ANDROID_LOG_ERROR, "MiniNative", "Failed to find LNI Java Method!");
        return;
    }

    jstring str = (*env)->NewStringUTF(env, buffer);
    (*env)->CallStaticVoidMethod(env, clazz, method, str);
    (*env)->DeleteLocalRef(env, str);
}

void send_log_buffer(const char *buffer) {
    __android_log_write(ANDROID_LOG_DEBUG, "libswordigo", buffer);
}
