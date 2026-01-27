#include "achievements.h"
#include "log.h"
#include "java.h"
#include <jni.h>

#define LOG_TAG "MiniAchievements"

void ach_register_all() {
	JNIEnv *env = get_jni_env();

	(*env)->CallStaticVoidMethod(env, g_Achievements, g_Achievements_registerAll);
}

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_NativeBridge_createAchievement(
	JNIEnv *env, jclass clazz,
	jstring jId, jstring jName, jstring jDescription, jint points,
	jstring jCounter, jint threshold
) {
	// id, name, description, points must not be null.

	const char *id = (*env)->GetStringUTFChars(env, jId, 0);
	const char *name = (*env)->GetStringUTFChars(env, jName, 0);
	const char *description = (*env)->GetStringUTFChars(env, jDescription, 0);
	const char *counter = NULL;
	if (jCounter != NULL)
		counter = (*env)->GetStringUTFChars(env, jCounter, 0);

	LOGD("Adding %p", shared_achievement_manager);
	create_achievement(
		shared_achievement_manager,
		id, name, description, points,
		counter, threshold
	);

	(*env)->ReleaseStringUTFChars(env, jId, id);
	(*env)->ReleaseStringUTFChars(env, jName, name);
	(*env)->ReleaseStringUTFChars(env, jDescription, description);
	if (jCounter != NULL)
		(*env)->ReleaseStringUTFChars(env, jCounter, counter);
}
