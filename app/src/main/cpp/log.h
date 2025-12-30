
#ifndef SWMINI_LOG_H
#define SWMINI_LOG_H

#include <android/log.h>

/** Log macros that can be used across the project. */

#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#ifdef NDK_DEBUG
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#else
#define LOGD(...)
#endif //NDK_DEBUG

int spawn_log_threads();

// Max length of log messages
// Longer than this will get split.
#define LOG_BUFFER_SIZE 1024

#endif //SWMINI_LOG_H
