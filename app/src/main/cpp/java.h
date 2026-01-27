#ifndef SWMINI_JAVA_H
#define SWMINI_JAVA_H

// This file stores the Java globals that are needed across SwMini's C codebase.

#include <jni.h>

extern JavaVM *g_jvm;  // Global JVM pointer

// Utility to get JNIEnv* for current thread (attaches if needed)
JNIEnv *get_jni_env();

// Utility to detach current thread if attached
void detach_jni_thread();


void cache_classes();

extern jclass g_StringWriter;
extern jmethodID g_StringWriter_init;
extern jmethodID g_StringWriter_toString;

extern jclass g_PrintWriter;
extern jmethodID g_PrintWriter_init;

extern jclass g_Arrays;
extern jmethodID g_Arrays_copyOfRange;

extern jclass g_Achievements;
extern jmethodID g_Achievements_registerAll;

#endif //SWMINI_JAVA_H
