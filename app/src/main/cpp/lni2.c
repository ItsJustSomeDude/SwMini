#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "lni2.h"
#include "log.h"

#define LOG_TAG "MiniLNI"

// Global variables for JNI environment and JVM
static JavaVM* g_jvm = NULL;
static pthread_key_t thread_key;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;
static jclass g_lni = NULL; // Global reference to NativeBridge class

static jmethodID g_getClass = NULL;
static jmethodID g_getMethod = NULL;
static jmethodID g_getSignature = NULL;

// Cleanup function for thread-specific JNI environment
static void detach_thread(void* env) {
    if (g_jvm && env) {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }
}

// Create thread-specific key for JNI environment
static void create_thread_key() {
    pthread_key_create(&thread_key, detach_thread);
}

// Get or attach JNI environment for the current thread
JNIEnv* get_jni_env() {
    JNIEnv* env = NULL;
    if (!g_jvm) {
        return NULL;
    }

    // Check if thread is already attached
    jint result = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if (result == JNI_EDETACHED) {
        // Attach thread and store env in thread-specific storage
        result = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
        if (result == JNI_OK) {
            pthread_setspecific(thread_key, env);
        } else {
            return NULL;
        }
    } else if (result != JNI_OK) {
        return NULL;
    }
    return env;
}

JNIEXPORT void JNICALL Java_net_itsjustsomedude_swrdg_LuaNativeInterface_init(JNIEnv *env, jclass clazz) {
    // Store JVM reference and initialize thread key
    (*env)->GetJavaVM(env, &g_jvm);
    pthread_once(&key_once, create_thread_key);

    // Store global reference to the class
    g_lni = (*env)->NewGlobalRef(env, clazz);

    g_getClass = (*env)->GetStaticMethodID(env, g_lni, "getClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    if (!g_getClass) LOGE("Failed to find method: %s", "getClass");

    g_getMethod = (*env)->GetStaticMethodID(env, g_lni, "getMethod", "(Ljava/lang/String;)Ljava/lang/String;");
    if (!g_getMethod) LOGE("Failed to find method: %s", "getMethod");

    g_getSignature = (*env)->GetStaticMethodID(env, g_lni, "getSignature", "(Ljava/lang/String;)Ljava/lang/String;");
    if (!g_getSignature) LOGE("Failed to find method: %s", "getSignature");
}

void idk(const char* method, const char* signature, ...) {
    // I have no idea...
    // Get the method from the name

    JNIEnv* thread_env = get_jni_env();
    if (!thread_env) { LOGE("Uhh"); return; }

    // Get the method + signature string

    char* ms = malloc(strlen(method) + strlen(signature) + 2);
    if(ms != NULL) {
        LOGE("Alloc Fail");
        return;
    }

    sprintf(ms, "%s:%s", method, signature);

    // Convert C string to jstring for Java call
    jstring jms = (*thread_env)->NewStringUTF(thread_env, ms);
    if (!jms) {
        free(ms);
        return;
    }

    jclass clazz = (*thread_env)->CallStaticObjectMethod(thread_env, g_lni, g_getClass, jms);

    jmethodID mid = (*thread_env)->GetStaticMethodID(thread_env, clazz, method, signature);
    if(mid == NULL) {
        LOGE("Failed to find method %s:%s!", method, signature);
        return;
    }

    jobject result = (*thread_env)->CallStaticObjectMethod(thread_env, clazz, mid);


//    jstring



    // Check for exceptions
    if ((*thread_env)->ExceptionCheck(thread_env)) {
        LOGE("Exception");
        (*thread_env)->ExceptionClear(thread_env);
        return;
    }



    free(ms);
}
