#include "core/java.h"
#include "jni.h"
#include "lauxlib.h"
#include "lni.h"
#include "core/log.h"
#include "lua.h"

#include <stdlib.h>

#define LOG_TAG "MiniLuaLNIExecute"

/* Executes `method` by name, using args on the top of the Lua stack. */
int lni_exec_name(lua_State *L, const char *method_name) {
	JNIEnv *env = miniJ_get_env();
	if (!env)
		return luaL_error(L, "LNI Internal Error: %s", "Failed to get JNI Env.");

	/* Number of Lua varargs */
	int nargs = lua_gettop(L);

	LOGD("Start of LNI Exec. Function: %s Lua Args: %i", method_name, nargs);

//	/* Loop through Lua args, make a string like "SNB" for the arg types. */
//	char overload_string[32];
//	for (int lua_i = 1; lua_i < nargs; lua_i++) {
//		if (lua_isboolean(L, lua_i))
//			overload_string[lua_i - 1] = 'B';
//		else if (lua_isnumber(L, lua_i))
//			overload_string[lua_i - 1] = 'N';
//		else if (lua_isstring(L, lua_i))
//			overload_string[lua_i - 1] = 'S';
//		else
//			return luaL_error(
//				L, "Argument %d is of type %s, which cannot be passed to LNI.",
//				lua_i, lua_typename(L, lua_type(L, lua_i))
//			);
//	}

	LNIMethod *method = lni_get_method(method_name);
	if (method == NULL)
		return luaL_error(L, "Unknown LNI Function: %s", method_name);

	// Fast Fail if wrong number of arguments.
	if (nargs != method->params_count)
		return luaL_error(
			L, "Wrong number of args for %s: expected %d, found %d",
			method_name, method->params_count, nargs
		);

	// Allocate jvalue array for JNI
	jvalue *args = malloc(nargs * sizeof(jvalue));
	if (!args)
		return luaL_error(L, "LNI Internal Error: %s", "Out of Memory.");

	for (int java_i = 0; java_i < nargs; java_i++) {
		int lua_i = java_i + 1; /* Lua, 1 indexed */

		int req_type = method->params[java_i];
		LOGD("Expecting an arg of type %d on lua index %d", req_type, lua_i);

		if (lua_isboolean(L, lua_i)) {
			if (req_type != LNI_TYPE_BOOL) {
				free(args);
				// TODO: All of this logic is wrong... It should be "unexpected boolean".
				// Will be fixed by the Overload rewrite though.
				return luaL_error(L, "Argument %d must be a boolean.", lua_i);
			}

			// LOGD("Pushed boolean to args array");
			args[java_i].z = lua_toboolean(L, lua_i);
		} else if (lua_isnumber(L, lua_i)) {
			if (req_type != LNI_TYPE_NUM) {
				free(args);
				return luaL_error(L, "Argument %d must be a number.", lua_i);
			}

			// LOGD("Pushed number to args array");
			args[java_i].d = lua_tonumber(L, lua_i);
		} else if (lua_isstring(L, lua_i)) {
			if (req_type != LNI_TYPE_STR) {
				free(args);
				return luaL_error(L, "Argument %d must be a string.", lua_i);
			}

			// LOGD("Pushed string to args array");
			const char *str = lua_tostring(L, lua_i);
			args[java_i].l = (*env)->NewStringUTF(env, str);
		} else {
			free(args);

			const char *t =
				req_type == LNI_TYPE_STR ? "string"
					: req_type == LNI_TYPE_NUM ? "number"
					: req_type == LNI_TYPE_BOOL ? "boolean"
						: "nil value";

			return luaL_error(L, "Argument %d must be a %s", lua_i, t);
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
		return luaL_error(
			L, "LNI Internal Error: %s %i", "Unknown Return type", method->returnType
		);
	}

	return 1;
}

/* Execute Lua: `ExecuteLNI("function name", ...)` */
int miniLL_lni_execute(lua_State *L) {
	const char *method_name = luaL_checkstring(L, 1);
	lua_remove(L, 1);

	return lni_exec_name(L, method_name);
}

static int lni_closure(lua_State *L) {
	const char *method_name = lua_tostring(L, lua_upvalueindex(1));
	return lni_exec_name(L, method_name);
}

int miniLL_lni_bind(lua_State *L) {
	const char *name = luaL_checkstring(L, 1);

	/* Store the name as upvalue */
	lua_pushstring(L, name);
	lua_pushcclosure(L, &lni_closure, 1);
	return 1;
}
