/* clang-format off */
#include "lni.h" /* clang-format on */

#include "jni.h"
#include "libs/khash.h"
#include "log.h"

#include <stdlib.h>
#include <string.h>

#define LOG_TAG "MiniLNICore"

KHASH_MAP_INIT_STR(str, LNIMethod*)

/**
 * Hash table to store all the registered LNI methods.
 */
static kh_str_t *methodsTable = NULL;
static khint_t k;

/**
 * `clazz` must be a global reference.
 */
static LNIMethod *createLNIMethod(
	jclass clazz,
	jmethodID mid,
	const int *params,
	size_t paramsLength,
	jint returnType
) {
	// Allocate memory for the struct
	LNIMethod *m = malloc(sizeof(LNIMethod));
	if (!m) return NULL;

	// Assign pointers
	m->clazz = clazz;
	m->mid = mid;
	m->paramsLength = paramsLength;
	m->returnType = returnType;

	// Allocate memory for the dynamic array
	m->params = malloc(paramsLength * sizeof(int));
	if (!m->params) {
		LOGE("Failed to allocate argument array.");
		free(m); // Clean up if allocation fails
		return NULL;
	}

	for (size_t i = 0; i < paramsLength; i++) {
//		LOGD("Register method thing %zu %d", i, params[i]);
		m->params[i] = params[i];
	}

	return m;
}

LNIMethod *get_lni_method(const char *methodName) {
	if (methodsTable == NULL) {
		// There are no LNI methods registered.
		return NULL;
	}

	// Look up the method in the table.
	k = kh_get_str(methodsTable, methodName);
	if (k == kh_end(methodsTable)) {
		// No match
		return NULL;
	}
	return kh_value(methodsTable, k);
}

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_LuaNativeInterface_nativeRegister(
	JNIEnv *env,
	jclass clazz,
	jclass targetClass,
	jstring jMethodName,
	jstring jSignature,
	jintArray jParams,
	jint returnType
) {
	// Ok, so we'll need to...
	// Unbox methodName
	// Unbox signature
	// Unbox params
	// Create method struct, store in table
	// free stuff.

	// Get methodName C string
	// MUST BE RELEASED
	const char *methodName = (*env)->GetStringUTFChars(env, jMethodName, 0);
	if (!methodName) {
		LOGE("Failed to convert method name.");
		return;
	}

	// Get signature C string
	// MUST BE RELEASED
	const char *signature = (*env)->GetStringUTFChars(env, jSignature, 0);
	if (!signature) {
		LOGE("Failed to convert signature for %s", methodName);
		(*env)->ReleaseStringUTFChars(env, jMethodName, methodName);
		return;
	}

	// Get params C array
	// MUST BE RELEASED
	jint *params = (*env)->GetIntArrayElements(env, jParams, NULL);
	if (params == NULL) {
		LOGE("Failed to convert params array for %s", methodName);
		(*env)->ReleaseStringUTFChars(env, jMethodName, methodName);
		(*env)->ReleaseStringUTFChars(env, jSignature, signature);
		return;
	}
	jsize paramsLength = (*env)->GetArrayLength(env, jParams);

	jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, methodName, signature);
	if (mid == NULL) {
		LOGE("Failed to find method id for %s", methodName);
		(*env)->ReleaseStringUTFChars(env, jMethodName, methodName);
		(*env)->ReleaseStringUTFChars(env, jSignature, signature);
		(*env)->ReleaseIntArrayElements(env, jParams, params, 0);
		return;
	}

	// Create global reference to target class
	jclass targetGlobal = (*env)->NewGlobalRef(env, targetClass);
	if (targetGlobal == NULL) {
		LOGE("Failed to bind reference to Target Class for method %s", methodName);
		(*env)->ReleaseStringUTFChars(env, jMethodName, methodName);
		(*env)->ReleaseStringUTFChars(env, jSignature, signature);
		(*env)->ReleaseIntArrayElements(env, jParams, params, 0);
		return;
	}

	LNIMethod *newMethod = createLNIMethod(targetGlobal, mid, params, paramsLength, returnType);
	if (newMethod == NULL) {
		LOGE("Failed to register LNI method %s", methodName);
		(*env)->ReleaseStringUTFChars(env, jMethodName, methodName);
		(*env)->ReleaseStringUTFChars(env, jSignature, signature);
		(*env)->ReleaseIntArrayElements(env, jParams, params, 0);
		return;
	}

	const char *methodNameKey = strdup(methodName);
	if (methodNameKey == NULL) {
		LOGE("Failed to store LNI method %s", methodName);
		(*env)->ReleaseStringUTFChars(env, jMethodName, methodName);
		(*env)->ReleaseStringUTFChars(env, jSignature, signature);
		(*env)->ReleaseIntArrayElements(env, jParams, params, 0);
		return;
	}

	// Create hash table if it does not exist.
	if (methodsTable == NULL) {
		methodsTable = kh_init_str();
	}

	int absent;
	k = kh_put_str(methodsTable, methodNameKey, &absent);
	if (absent) {
		LOGD("Registering new method %s to table", methodNameKey);
		kh_value(methodsTable, k) = newMethod;
	}

	(*env)->ReleaseStringUTFChars(env, jMethodName, methodName);
	(*env)->ReleaseStringUTFChars(env, jSignature, signature);
	(*env)->ReleaseIntArrayElements(env, jParams, params, 0);
}
