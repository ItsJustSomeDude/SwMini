#ifndef SWMINI_JAVA_H
#define SWMINI_JAVA_H

// This file stores the Java globals that are needed across SwMini's C codebase.

#include <jni.h>

void cache_classes();

extern jclass g_StringWriter;
extern jmethodID g_StringWriter_init;
extern jmethodID g_StringWriter_toString;

extern jclass g_PrintWriter;
extern jmethodID g_PrintWriter_init;

extern jclass g_Arrays;
extern jmethodID g_Arrays_copyOfRange;

#endif //SWMINI_JAVA_H
