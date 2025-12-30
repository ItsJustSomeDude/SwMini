#ifndef SWMINI_JNI_H
#define SWMINI_JNI_H

#include <jni.h>

extern JavaVM *g_jvm;  // Global JVM pointer

// Utility to get JNIEnv* for current thread (attaches if needed)
JNIEnv *get_jni_env();

// Utility to detach current thread if attached
void detach_jni_thread();

#endif //SWMINI_JNI_H
