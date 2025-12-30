#include <malloc.h>
#include "lni.h"
#include "../library.h"
#include "../../jni.h"
#include "../../log.h"
#include "../../java.h"

#define LOG_TAG "MiniLuaLNIExecute"

int lni_execute(lua_State *L) {
	JNIEnv *env = get_jni_env();
	if (!env) {
		LOGE("Failed to get JNI Env");
		return 0;
	}

//	LOGD(
//		"stack height: %d, types: %s %s %s %s",
//		lua_gettop(L),
//		lua_typename(L, lua_type(L, 0)),
//		lua_typename(L, lua_type(L, 1)),
//		lua_typename(L, lua_type(L, 2)),
//		lua_typename(L, lua_type(L, 3))
//	);

	LOGD("At the start of LNI call, stack height is %i", lua_gettop(L));

	// Check if first argument is method to call
	if (!lua_isstring(L, 1)) {
		return luaL_error(L, "First argument must be a string");;
	}

	// Get the first string argument
	const char *methodName = lua_tostring(L, 1);

	// Count number of varargs
	int nargs = lua_gettop(L) - 1;

	// TODO: Fetch the Lua Args here so we can find the right Overload.

	LNIMethod *method = get_lni_method(methodName);
	if (method == NULL) {
		return luaL_error(L, "Unknown LNI Function %s", methodName);
	}

	// Fast Fail if wrong number of arguments.
	if (nargs != method->paramsLength) {
		return luaL_error(
			L,
			"Wrong number of args for %s: expected %d, found %d",
			methodName, method->paramsLength, nargs
		);
	}

	// Allocate jvalue array for JNI
	jvalue *args = malloc(nargs * sizeof(jvalue));
	if (!args) {
		free(args);
		return luaL_error(L, "LNI Failure: OOM");;
	}

	// Convert Lua arguments to JNI jvalue array
	for (int i = 0; i < nargs; i++) {
		LOGD("i: %d, stack height: %d, types: %s %s %s", i, lua_gettop(L), lua_typename(L, 1),
		     lua_typename(L, 2), lua_typename(L, 3));
		int lua_idx = i + 2; // Lua stack index (skip first string arg)

		int reqType = method->params[i];

		LOGD("Expecting an arg of type %d on lua index %d", reqType, lua_idx);

		if (lua_isboolean(L, lua_idx)) {
			if (reqType != LNI_TYPE_BOOL) {
				free(args);
				// TODO: All of this logic is wrong... It should be "unexpected boolean".
				// Will be fixed by the Overload rewrite though.
				return luaL_error(L, "Argument %d must be a boolean.", i + 1);
			}

			// LOGD("Pushed boolean to args array");
			args[i].z = lua_toboolean(L, lua_idx);
		} else if (lua_isnumber(L, lua_idx)) {
			if (reqType != LNI_TYPE_NUM) {
				free(args);
				return luaL_error(L, "Argument %d must be a number.", i + 1);
			}

			// LOGD("Pushed number to args array");
			args[i].d = lua_tonumber(L, lua_idx);
		} else if (lua_isstring(L, lua_idx)) {
			if (reqType != LNI_TYPE_STR) {
				free(args);
				return luaL_error(L, "Argument %d must be a string.", i + 1);
			}

			// LOGD("Pushed string to args array");
			const char *str = lua_tostring(L, lua_idx);
			args[i].l = (*env)->NewStringUTF(env, str);
		} else {
			free(args);

			const char *t =
				reqType == LNI_TYPE_STR ? "string"
					: reqType == LNI_TYPE_NUM ? "number"
					: reqType == LNI_TYPE_BOOL ? "boolean"
						: "nil value";

			return luaL_error(L, "Argument %d must be a %s", i + 1, t);
		}
	}

	jboolean boolResult;
	jdouble doubleResult;
	jstring stringResult;
	if (method->returnType == LNI_TYPE_VOID)
		(*env)->CallStaticVoidMethodA(env, method->clazz, method->mid, args);
	else if (method->returnType == LNI_TYPE_BOOL)
		boolResult = (*env)->CallStaticBooleanMethodA(env, method->clazz, method->mid, args);
	else if (method->returnType == LNI_TYPE_NUM)
		doubleResult = (*env)->CallStaticDoubleMethodA(env, method->clazz, method->mid, args);
	else if (method->returnType == LNI_TYPE_STR)
		stringResult = (*env)->CallStaticObjectMethodA(env, method->clazz, method->mid, args);

	// Clean up jstring objects
	for (int i = 0; i < nargs; i++) {
		if (method->params[i] == LNI_TYPE_STR)
			(*env)->DeleteLocalRef(env, args[i].l);
	}
	free(args);

	// Check for JNI exceptions
	jthrowable exc = (*env)->ExceptionOccurred(env);
	if (exc) {
		// Clear the exception so it doesn't interfere with further JNI calls
		(*env)->ExceptionClear(env);

		// Capture exception description
		jobject string_writer = (*env)->NewObject(env, g_StringWriter, g_StringWriter_init);
		jobject print_writer =
			(*env)->NewObject(env, g_PrintWriter, g_PrintWriter_init, string_writer);

		jclass exception_class = (*env)->GetObjectClass(env, exc);
		jmethodID getStackTrace = (*env)->GetMethodID(
			env, exception_class, "getStackTrace", "()[Ljava/lang/StackTraceElement;"
		);
		jmethodID setStackTrace = (*env)->GetMethodID(
			env, exception_class, "setStackTrace", "([Ljava/lang/StackTraceElement;)V"
		);
		jmethodID printStackTrace = (*env)->GetMethodID(
			env, exception_class, "printStackTrace", "(Ljava/io/PrintWriter;)V"
		);

		// StackTraceElement[] stackFrames = exc.getStackTrace();
		jobjectArray stackFrames = (*env)->CallObjectMethod(env, exc, getStackTrace);
		// int numFrames = stackFrames.length;
		jint numFrames = (*env)->GetArrayLength(env, stackFrames);
		// There _should_ almost always be 4 "com.touchfoo..." internal lines on the bottom of the stack.
		jint desiredFrames = numFrames - 4;
		// If we ate all the lines, re-add all of them.
		if (desiredFrames < 0) desiredFrames = numFrames;

		// StackTraceElement[] shortStack = Arrays.copyOfRange(stackFrames, 0, desiredFrames);
		jobjectArray shortStack =
			(*env)->CallStaticObjectMethod(
				env,
				g_Arrays, g_Arrays_copyOfRange,
				stackFrames, 0, desiredFrames
			);

		// exc.setStackTrace(shortStack);
		// exc.printStackTrace(print_writer);
		(*env)->CallVoidMethod(env, exc, setStackTrace, shortStack);
		(*env)->CallVoidMethod(env, exc, printStackTrace, print_writer);

		// String exc_string = string_writer.toString();
		jstring exc_string =
			(jstring) (*env)->CallObjectMethod(env, string_writer, g_StringWriter_toString);
		const char *exc_str = (*env)->GetStringUTFChars(env, exc_string, NULL);

		lua_pushstring(L, exc_str);

		// Clean up JNI references
		(*env)->ReleaseStringUTFChars(env, exc_string, exc_str);
		(*env)->DeleteLocalRef(env, exc_string);
		(*env)->DeleteLocalRef(env, print_writer);
		(*env)->DeleteLocalRef(env, string_writer);
		(*env)->DeleteLocalRef(env, stackFrames);
		(*env)->DeleteLocalRef(env, shortStack);
		(*env)->DeleteLocalRef(env, exc);

		return lua_error(L);
	}

	if (method->returnType == LNI_TYPE_VOID)
		return 0;
	else if (method->returnType == LNI_TYPE_BOOL)
		lua_pushboolean(L, boolResult);
	else if (method->returnType == LNI_TYPE_NUM)
		lua_pushnumber(L, doubleResult);
	else if (method->returnType == LNI_TYPE_STR) {
		const char *str = (*env)->GetStringUTFChars(env, stringResult, NULL);
		lua_pushstring(L, str);
		(*env)->ReleaseStringUTFChars(env, stringResult, str);
		(*env)->DeleteLocalRef(env, stringResult);
	} else {
		return luaL_error(L, "Um, error maybe? idk. Nil result.");
	}

	LOGD("At the end of the LNI call, stack height is %i", lua_gettop(L));

	// Value is on the top of the stack. Return it to Lua.
	return 1;
}
