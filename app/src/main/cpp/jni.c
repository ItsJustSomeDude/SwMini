#include "java.h"
#include "main.h"

#include <jni.h>
#include <pthread.h> // For thread-local storage if needed
#include <stddef.h>

#define LOG_TAG "MiniJNI"

JavaVM *g_jvm = NULL;

// Thread-local flag to track attachment (optional, for detach logic)
static pthread_key_t s_thread_key;

static void thread_destructor(void *data) {
	if (data) detach_jni_thread();
}

static void init_thread_key() {
	pthread_key_create(&s_thread_key, thread_destructor);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
	g_jvm = vm;

	JNIEnv *env = NULL;
	if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_6) != JNI_OK) return JNI_ERR;

	init_thread_key();  // If using thread-local tracking

	earlyLoad();

	return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_NativeBridge_midLoad(JNIEnv *env, jclass clazz) {
	midLoad();
}

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_NativeBridge_lateLoad(JNIEnv *env, jclass clazz) {
	lateLoad();
}

JNIEnv *get_jni_env() {
	JNIEnv *env = NULL;
	if ((*g_jvm)->GetEnv(g_jvm, (void **) &env, JNI_VERSION_1_6) == JNI_OK) {
		return env;  // Already attached
	}
	// Attach (as daemon for background threads)
	jint result = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
	if (result != JNI_OK) return NULL;

	// Mark as attached for later detach (optional)
	pthread_setspecific(s_thread_key, (void *) 1);

	return env;
}

void detach_jni_thread() {
	(*g_jvm)->DetachCurrentThread(g_jvm);
	pthread_setspecific(s_thread_key, NULL);
}
