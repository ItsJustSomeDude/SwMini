#include "java.h"
#include "jni.h"

jclass g_StringWriter;
jmethodID g_StringWriter_init;
jmethodID g_StringWriter_toString;

jclass g_PrintWriter;
jmethodID g_PrintWriter_init;

jclass g_Arrays;
jmethodID g_Arrays_copyOfRange;

jclass g_Achievements;
jmethodID g_Achievements_registerAll;

/** Create references to all the Java classes and Methods we're going to need. */
void cache_classes() {
	JNIEnv *env = get_jni_env();

	g_StringWriter = (*env)->NewGlobalRef(env, (*env)->FindClass(env, "java/io/StringWriter"));
	g_StringWriter_init = (*env)->GetMethodID(env, g_StringWriter, "<init>", "()V");
	g_StringWriter_toString = (*env)->GetMethodID(
		env,
		g_StringWriter,
		"toString",
		"()Ljava/lang/String;"
	);

	g_PrintWriter = (*env)->NewGlobalRef(env, (*env)->FindClass(env, "java/io/PrintWriter"));
	g_PrintWriter_init = (*env)->GetMethodID(env, g_PrintWriter, "<init>", "(Ljava/io/Writer;)V");

	g_Arrays = (*env)->NewGlobalRef(env, (*env)->FindClass(env, "java/util/Arrays"));
	g_Arrays_copyOfRange = (*env)->GetStaticMethodID(
		env,
		g_Arrays,
		"copyOfRange",
		"([Ljava/lang/Object;II)[Ljava/lang/Object;"
	);

	g_Achievements = (*env)->NewGlobalRef(
		env, (*env)->FindClass(env, "net/itsjustsomedude/swrdg/Achievements")
	);
	g_Achievements_registerAll = (*env)->GetStaticMethodID(
		env, g_Achievements, "registerAll", "()V"
	);
}
