#ifndef NEEDLE_JNI_H
#define NEEDLE_JNI_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEnv* get_jni_env();

JNIEXPORT void JNICALL Java_net_itsjustsomedude_swrdg_NativeBridge_init(JNIEnv *, jclass);

#ifdef __cplusplus
}
#endif

#endif //NEEDLE_JNI_H
