#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "lni2.h"
#include "log.h"

#include "lua/lua.h"
#include "lauxlib.h"

#include "libs/khash.h"

#define LOG_TAG "MiniLNI"

// Global variables for JNI environment and JVM
static JavaVM* g_jvm = NULL;
static pthread_key_t thread_key;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;
static jclass g_lni = NULL;

//static jmethodID g_getClass = NULL;
//static jmethodID g_getSignature = NULL;
//static jmethodID g_getParams = NULL;

typedef struct {
    jclass clazz;       // Pointer to jclass owning this method
    jmethodID mid;      // Pointer to method ID
    int* params;        // Pointer to dynamic array of integers
    size_t paramsLength;

    int returnType;

    // We don't need the signature, or the name, because the name will be the hash table key,
    // And since we have the methodID we don't need the signature.
} LNIMethod;

// See the LuaNativeInterface::getLNIType method.
#define LNI_TYPE_VOID 0
#define LNI_TYPE_BOOL 1
#define LNI_TYPE_NUM 2
#define LNI_TYPE_STR 3

KHASH_MAP_INIT_STR(str, LNIMethod*)

//#region JNI Boilerplate

// Cleanup function for thread-specific JNI environment
static void detach_thread(void* env) {
    if (g_jvm && env) {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }
}

// Create thread-specific key for JNI environment
static void create_thread_key() {
    pthread_key_create(&thread_key, detach_thread);
}

// Get or attach JNI environment for the current thread
JNIEnv* get_jni_env() {
    JNIEnv* env = NULL;
    if (!g_jvm) {
        return NULL;
    }

    // Check if thread is already attached
    jint result = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if (result == JNI_EDETACHED) {
        // Attach thread and store env in thread-specific storage
        result = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
        if (result == JNI_OK) {
            pthread_setspecific(thread_key, env);
        } else {
            return NULL;
        }
    } else if (result != JNI_OK) {
        return NULL;
    }
    return env;
}

/**
 * Hash table to store all the registered LNI methods.
 */
static kh_str_t *methodsTable;
static khint_t k;

JNIEXPORT void JNICALL Java_net_itsjustsomedude_swrdg_LuaNativeInterface_nativeInit(JNIEnv *env, jclass clazz) {
    // Store JVM reference and initialize thread key
    (*env)->GetJavaVM(env, &g_jvm);
    pthread_once(&key_once, create_thread_key);

    // Store global reference to the class
    g_lni = (*env)->NewGlobalRef(env, clazz);

//    g_getClass = (*env)->GetStaticMethodID(env, g_lni, "getClass", "(Ljava/lang/String;)Ljava/lang/Class;");
//    if (!g_getClass) LOGE("Failed to find method: %s", "getClass");
//
//    g_getSignature = (*env)->GetStaticMethodID(env, g_lni, "getSignature", "(Ljava/lang/String;)Ljava/lang/String;");
//    if (!g_getSignature) LOGE("Failed to find method: %s", "getSignature");
//
//    g_getParams = (*env)->GetStaticMethodID(env, g_lni, "getParams", "(Ljava/lang/String;)[I");
//    if (!g_getParams) LOGE("Failed to find method: %s", "getParams");

    methodsTable = kh_init_str();
}

//#endregion

LNIMethod* createLNIMethod(jclass clazz, jmethodID mid, const int* params, size_t paramsLength, jint returnType) {
    // Allocate memory for the struct
    LNIMethod* m = malloc(sizeof(LNIMethod));
    if (!m) return NULL;

    // Assign pointers
    m->clazz = clazz;
    m->mid = mid;
    m->paramsLength = paramsLength;
    m->returnType = returnType;

    // Allocate memory for the dynamic array
    m->params = malloc(paramsLength * sizeof(int));
    if (!m->params) {
        free(m); // Clean up if allocation fails
        return NULL;
    }

    for (size_t i = 0; i < paramsLength; i++) {
        m->params[i] = params[i];
    }

    return m;
}

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_LuaNativeInterface_register(
        JNIEnv *env, jclass clazz, jclass targetClass, jstring jMethodName,
        jstring jSignature, jintArray jParams, jint returnType
) {
    // Ok, so we'll need to...
    // Unbox methodName
    // Unbox signature
    // Unbox params
    // Create method struct, store in table
    // free stuff.

    // Get methodName C string
    // MUST BE RELEASED
    const char* methodName = (*env)->GetStringUTFChars(env, jMethodName, 0);
    if(!methodName) {
        LOGE("Failed to convert method name.");
        return;
    }

    // Get signature C string
    // MUST BE RELEASED
    const char* signature = (*env)->GetStringUTFChars(env, jSignature, 0);
    if(!signature) {
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
    if(mid == NULL) {
        LOGE("Failed to find method id for %s", methodName);
        (*env)->ReleaseStringUTFChars(env, jMethodName, methodName);
        (*env)->ReleaseStringUTFChars(env, jSignature, signature);
        (*env)->ReleaseIntArrayElements(env, jParams, params, 0);
        return;
    }

    // Create global reference to target class
    jclass targetGlobal = (*env)->NewGlobalRef(env, targetClass);
    if(targetGlobal == NULL) {
        LOGE("Failed to bind reference to Target Class for method %s", methodName);
        (*env)->ReleaseStringUTFChars(env, jMethodName, methodName);
        (*env)->ReleaseStringUTFChars(env, jSignature, signature);
        (*env)->ReleaseIntArrayElements(env, jParams, params, 0);
        return;
    }

    LNIMethod* newMethod = createLNIMethod(targetGlobal, mid, params, paramsLength, returnType);
    if(newMethod == NULL) {
        LOGE("Failed to register LNI method %s", methodName);
        (*env)->ReleaseStringUTFChars(env, jMethodName, methodName);
        (*env)->ReleaseStringUTFChars(env, jSignature, signature);
        (*env)->ReleaseIntArrayElements(env, jParams, params, 0);
        return;
    }

    const char* methodNameKey = strdup(methodName);
    if(methodNameKey == NULL) {
        LOGE("Failed to store LNI method %s", methodName);
        (*env)->ReleaseStringUTFChars(env, jMethodName, methodName);
        (*env)->ReleaseStringUTFChars(env, jSignature, signature);
        (*env)->ReleaseIntArrayElements(env, jParams, params, 0);
        return;
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

int executeLNI(lua_State *L) {
    JNIEnv *env = get_jni_env();
    if (!env) {
        LOGE("Failed to get JNI Env");
        return 0;
    }

    LOGD(
            "stack height: %d, types: %s %s %s %s",
            lua_gettop(L),
            lua_typename(L, lua_type(L, 0)),
            lua_typename(L, lua_type(L, 1)),
            lua_typename(L, lua_type(L, 2)),
            lua_typename(L, lua_type(L, 3))
    );

    // Check if first argument is method to call
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "First argument must be a string");
        return 0;
    }

    // Get the first string argument
    const char *methodName = lua_tostring(L, 1);

    // Look up the method in the table.
    k = kh_get_str(methodsTable, methodName);
    if(k == kh_end(methodsTable)) {
        // No match
        luaL_error(L, "Unknown LNI Function %s", methodName);
        return 0;
    }
    LNIMethod *method = kh_value(methodsTable, k);

    // Count number of varargs
    int nargs = lua_gettop(L) - 1;

    // Fast Fail if wrong number of arguments.
    if(nargs != method->paramsLength) {
        luaL_error(L, "Wrong number of args for %s: expected %d, found %d", methodName, method->paramsLength, nargs);
        return 0;
    }

    // Allocate jvalue array for JNI
    jvalue *args = (jvalue *)malloc(nargs * sizeof(jvalue));
    if (!args) {
        free(args);
        luaL_error(L, "LNI Failure: OOM");
        return 0;
    }

    // Convert Lua arguments to JNI jvalue array
    for (int i = 0; i < nargs; i++) {
        LOGD("i: %d, stack height: %d, types: %s %s %s", i, lua_gettop(L), lua_typename(L, 1), lua_typename(L, 2), lua_typename(L, 3));
        int lua_idx = i + 2; // Lua stack index (skip first string arg)

        int reqType = method->params[i];

        LOGD("Expecting an arg of type %d", reqType);

        if (lua_isstring(L, lua_idx)) {
            if(reqType != LNI_TYPE_STR) {
                luaL_error(L, "Argument %d must be a string.", i + 1);
                free(args);
                return 0;
            }

            const char *str = lua_tostring(L, lua_idx);
            args[i].l = (*env)->NewStringUTF(env, str);
        }
        else if (lua_isnumber(L, lua_idx)) {
            if(reqType != LNI_TYPE_NUM) {
                luaL_error(L, "Argument %d must be a number.", i + 1);
                free(args);
                return 0;
            }

            // TODO: This will likely break if the method wants an int/float/short.
            args[i].d = lua_tonumber(L, lua_idx);
        }
        else if (lua_isboolean(L, lua_idx)) {
            if(reqType != LNI_TYPE_BOOL) {
                luaL_error(L, "Argument %d must be a boolean.", i + 1);
                free(args);
                return 0;
            }

            args[i].z = lua_toboolean(L, lua_idx);
        }
        else {
            free(args);
            luaL_error(
                    L,
                    "Bottom: Argument %d must be a %s",
                    i + 1,
                    reqType == LNI_TYPE_STR ? "string"
                    : reqType == LNI_TYPE_NUM ? "number"
                    : reqType == LNI_TYPE_BOOL ? "boolean"
                    : "nil value"
            );
            return 0;
        }
    }

    jobject result = NULL;
    if(method->returnType == LNI_TYPE_VOID)
        (*env)->CallStaticVoidMethodA(env, method->clazz, method->mid, args);
    else
        result = (*env)->CallStaticObjectMethodA(env, method->clazz, method->mid, args);

    // Clean up jstring objects
    for (int i = 0; i < nargs; i++) {
        if (lua_isstring(L, i + 2)) {
            (*env)->DeleteLocalRef(env, args[i].l);
        }
    }
    free(args);

    // Check for JNI exceptions
    jthrowable exc = (*env)->ExceptionOccurred(env);
    if (exc) {
        // Clear the exception so it doesn't interfere with further JNI calls
        (*env)->ExceptionClear(env);

        // Capture exception description
        jclass string_writer_class = (*env)->FindClass(env, "java/io/StringWriter");
        jmethodID string_writer_init = (*env)->GetMethodID(env, string_writer_class, "<init>", "()V");
        jobject string_writer = (*env)->NewObject(env, string_writer_class, string_writer_init);

        jclass print_writer_class = (*env)->FindClass(env, "java/io/PrintWriter");
        jmethodID print_writer_init = (*env)->GetMethodID(env, print_writer_class, "<init>", "(Ljava/io/Writer;)V");
        jobject print_writer = (*env)->NewObject(env, print_writer_class, print_writer_init, string_writer);

        jmethodID print_stack_trace = (*env)->GetMethodID(env, (*env)->GetObjectClass(env, exc), "printStackTrace", "(Ljava/io/PrintWriter;)V");
        (*env)->CallVoidMethod(env, exc, print_stack_trace, print_writer);

        jmethodID to_string = (*env)->GetMethodID(env, string_writer_class, "toString", "()Ljava/lang/String;");
        jstring exc_string = (jstring)(*env)->CallObjectMethod(env, string_writer, to_string);

        const char *exc_str = (*env)->GetStringUTFChars(env, exc_string, NULL);
        luaL_error(L, exc_str);

        // Clean up JNI references
        (*env)->ReleaseStringUTFChars(env, exc_string, exc_str);
        (*env)->DeleteLocalRef(env, exc_string);
        (*env)->DeleteLocalRef(env, print_writer);
        (*env)->DeleteLocalRef(env, string_writer);
        (*env)->DeleteLocalRef(env, exc);
        return 1;
    }

    // void method called or method returned null, send nil back to Lua.
    if(result == NULL) return 0;

    // Push result to Lua stack (assuming object can be converted)
    jclass string_class = (*env)->FindClass(env, "java/lang/String");
    jclass number_class = (*env)->FindClass(env, "java/lang/Number");
    jclass boolean_class = (*env)->FindClass(env, "java/lang/Boolean");

    // Check if result is a String
    if ((*env)->IsInstanceOf(env, result, string_class)) {
        const char *str_result = (*env)->GetStringUTFChars(env, (jstring)result, NULL);
        lua_pushstring(L, str_result);
        (*env)->ReleaseStringUTFChars(env, (jstring)result, str_result);
    }
    // Check if result is a Number
    else if ((*env)->IsInstanceOf(env, result, number_class)) {
        jmethodID double_value = (*env)->GetMethodID(env, number_class, "doubleValue", "()D");
        jdouble num_result = (*env)->CallDoubleMethod(env, result, double_value);
        lua_pushnumber(L, num_result);
    }
    // Check if result is a Boolean
    else if ((*env)->IsInstanceOf(env, result, boolean_class)) {
        jmethodID boolean_value = (*env)->GetMethodID(env, boolean_class, "booleanValue", "()Z");
        jboolean bool_result = (*env)->CallBooleanMethod(env, result, boolean_value);
        lua_pushboolean(L, bool_result);
    }
    else {
        luaL_error(L, "Um, error maybe? idk. Nil result.");
    }
//        // Fallback to toString
//    else {
//        jstring jstr = (*env)->CallObjectMethod(env, result, (*env)->GetMethodID(env, (*env)->GetObjectClass(env, result), "toString", "()Ljava/lang/String;"));
//        const char *str_result = (*env)->GetStringUTFChars(env, jstr, NULL);
//        lua_pushstring(L, str_result);
//        (*env)->ReleaseStringUTFChars(env, jstr, str_result);
//        (*env)->DeleteLocalRef(env, jstr);
//    }

    // Clean up class references and result
    (*env)->DeleteLocalRef(env, string_class);
    (*env)->DeleteLocalRef(env, number_class);
    (*env)->DeleteLocalRef(env, boolean_class);
    (*env)->DeleteLocalRef(env, result);
    return 1;

}

// Old stuff...
///**
// * Gets the jclass containing the passed methodName
// * TODO: Caching?
// */
//jclass getClass(const char* methodName) {
//    JNIEnv* env = get_jni_env();
//    if (!env) {
//        LOGE("Failed to get JNI Env");
//        return NULL;
//    }
//
//    // Convert C methodName to jstring
//    jstring jMethodName = (*env)->NewStringUTF(env, methodName);
//    if (!jMethodName) {
//        LOGE("Allocation error in method id query.");
//        return NULL;
//    }
//
//    // This gets the clazz that holds the static method we want to call
//    jclass clazz = (*env)->CallStaticObjectMethod(env, g_lni, g_getClass, jMethodName);
//    if(!clazz) {
//        LOGE("Invalid clazz for method %s", methodName);
//        return NULL;
//    }
//
//    return clazz;
//}
//
///**
// * Queries all the registered LNI functions for the one with this name.
// * Uses cache if possible (eventually).
// */
//jmethodID getMethodID(const char* methodName) {
//    JNIEnv* env = get_jni_env();
//    if (!env) {
//        LOGE("Failed to get JNI Env");
//        return NULL;
//    }
//
//    // Convert C methodName to jstring
//    jstring jMethodName = (*env)->NewStringUTF(env, methodName);
//    if (!jMethodName) {
//        LOGE("Allocation error in method id query.");
//        return NULL;
//    }
//
//    // This gets the clazz that holds the static method we want to call
//    jclass clazz = (*env)->CallStaticObjectMethod(env, g_lni, g_getClass, jMethodName);
//    if(!clazz) {
//        LOGE("Invalid clazz for method %s", methodName);
//        return NULL;
//    }
//
//    // Next we need the signature as a jstring...
//    jstring jSignature = (*env)->CallStaticObjectMethod(env, g_lni, g_getSignature, jMethodName);
//    if(!jSignature) {
//        LOGE("Unknown signature for method %s", methodName);
//        return NULL;
//    }
//
//    // ...and then convert to C string.
//    // ALWAYS CLEANUP: 1
//    const char* signature = (*env)->GetStringUTFChars(env, jSignature, 0);
//    if(!signature) {
//        LOGE("Failed to convert signature for %s", methodName);
//        return NULL;
//    }
//
//    // We now have the clazz, method name (passed), and signature.
//    // Get the method ID now.
//    jmethodID mid = (*env)->GetStaticMethodID(env, clazz, methodName, signature);
//    if(mid == NULL) {
//        (*env)->ReleaseStringUTFChars(env, jMethodName, methodName);
//        LOGE("Failed to find method %s!", methodName);
//        return NULL;
//    }
//
//    (*env)->ReleaseStringUTFChars(env, jMethodName, methodName);
//
//    LOGD("Found method ID for %s: %p", methodName, mid);
//    return mid;
//}
//
//int* getParamTypes(const char* methodName) {
//    JNIEnv* env = get_jni_env();
//    if (!env) {
//        LOGE("Failed to get JNI Env");
//        return NULL;
//    }
//
//    jintArray array = (jintArray)(*env)->CallStaticObjectMethod(env, g_lni, g_getParams);
//    if (array == NULL) {
//        LOGE("Invalid Param Array");
//        return NULL;
//    }
//
//    // Get the elements of the array
//    jint *elements = (*env)->GetIntArrayElements(env, array, NULL);
//    if (elements == NULL) {
//        LOGE("Failed to get array elements");
//        return NULL;
//    }
//
//    // Get the array length
//    jsize length = (*env)->GetArrayLength(env, array);
//
//    // Process the array (e.g., print elements)
//    for (int i = 0; i < length; i++) {
//        printf("Element %d: %d\n", i, elements[i]);
//    }
//
//    // Release the array elements
//    (*env)->ReleaseIntArrayElements(env, array, elements, 0);
//
//
//}
//int callLNI(lua_State *L) {
//    JNIEnv* env = get_jni_env();
//    if (!env) {
//        LOGE("Failed to get JNI Env");
//        return 0;
//    }
//
//    // Check if first argument is method to call
//    if (!lua_isstring(L, 1)) {
//        luaL_error(L, "First argument must be a string");
//        return 0;
//    }
//
//    // Get the first string argument
//    const char *methodName = lua_tostring(L, 1);
//
//    // Count number of varargs
//    int nargs = lua_gettop(L) - 1;
//
//    // Allocate jvalue array for JNI
//    jvalue *args = (jvalue *)malloc(nargs * sizeof(jvalue));
//    if (!args) {
//        luaL_error(L, "Memory allocation failed");
//        return 0;
//    }
//
//    // Get paramTypes array from Java.
//
//    // Convert Lua arguments to JNI jvalue array
//    for (int i = 0; i < nargs; i++) {
//        int lua_idx = i + 2; // Lua stack index (skip first string arg)
//
//        // TODO: Fetch and check against the Params array from Java, and return errors to Lua if not matching.
//
//        if (lua_isstring(L, lua_idx)) {
//            const char *str = lua_tostring(L, lua_idx);
//            args[i].l = (*env)->NewStringUTF(env, str);
//        }
//        else if (lua_isnumber(L, lua_idx)) {
//            // TODO: This will likely break if the method wants an int/float/short.
//            args[i].d = lua_tonumber(L, lua_idx);
//        }
//        else if (lua_isboolean(L, lua_idx)) {
//            args[i].z = lua_toboolean(L, lua_idx);
//        }
//        else {
//            free(args);
//            luaL_error(L, "Argument %d must be string, number, or boolean", i + 1);
//            return 0;
//        }
//    }
//
//    jmethodID mid = getMethodID(methodName);
//    if (mid == NULL) {
//        free(args);
//        luaL_error(L, "LNI Method %s not found", methodName);
//        return 0;
//    }
//
//    jclass clazz = getClass(methodName);
//    if (clazz == NULL) {
//        free(args);
//        luaL_error(L, "LNI Class %s not found", methodName);
//        return 0;
//    }
//
//    // Call JNI method
//    jobject result = (*env)->CallStaticObjectMethodA(env, clazz, mid, args);
//
//    // Clean up jstring objects
//    for (int i = 0; i < nargs; i++) {
//        if (lua_isstring(L, i + 2)) {
//            (*env)->DeleteLocalRef(env, args[i].l);
//        }
//    }
//    free(args);
//
//    // Check for JNI exceptions
//    jthrowable exc = (*env)->ExceptionOccurred(env);
//    if (exc) {
//        // Clear the exception so it doesn't interfere with further JNI calls
//        (*env)->ExceptionClear(env);
//
//        // Capture exception description
//        jclass string_writer_class = (*env)->FindClass(env, "java/io/StringWriter");
//        jmethodID string_writer_init = (*env)->GetMethodID(env, string_writer_class, "<init>", "()V");
//        jobject string_writer = (*env)->NewObject(env, string_writer_class, string_writer_init);
//
//        jclass print_writer_class = (*env)->FindClass(env, "java/io/PrintWriter");
//        jmethodID print_writer_init = (*env)->GetMethodID(env, print_writer_class, "<init>", "(Ljava/io/Writer;)V");
//        jobject print_writer = (*env)->NewObject(env, print_writer_class, print_writer_init, string_writer);
//
//        jmethodID print_stack_trace = (*env)->GetMethodID(env, (*env)->GetObjectClass(env, exc), "printStackTrace", "(Ljava/io/PrintWriter;)V");
//        (*env)->CallVoidMethod(env, exc, print_stack_trace, print_writer);
//
//        jmethodID to_string = (*env)->GetMethodID(env, string_writer_class, "toString", "()Ljava/lang/String;");
//        jstring exc_string = (jstring)(*env)->CallObjectMethod(env, string_writer, to_string);
//
//        const char *exc_str = (*env)->GetStringUTFChars(env, exc_string, NULL);
//        luaL_error(L, exc_str);
//
//        // Clean up JNI references
//        (*env)->ReleaseStringUTFChars(env, exc_string, exc_str);
//        (*env)->DeleteLocalRef(env, exc_string);
//        (*env)->DeleteLocalRef(env, print_writer);
//        (*env)->DeleteLocalRef(env, string_writer);
//        (*env)->DeleteLocalRef(env, exc);
//        return 1;
//    }
//
//    // void method called or method returned null, send nil back to Lua.
//    if(result == NULL) return 0;
//
//    // Push result to Lua stack (assuming object can be converted)
//
//    jclass string_class = (*env)->FindClass(env, "java/lang/String");
//    jclass number_class = (*env)->FindClass(env, "java/lang/Number");
//    jclass boolean_class = (*env)->FindClass(env, "java/lang/Boolean");
//
//    // Check if result is a String
//    if ((*env)->IsInstanceOf(env, result, string_class)) {
//        const char *str_result = (*env)->GetStringUTFChars(env, (jstring)result, NULL);
//        lua_pushstring(L, str_result);
//        (*env)->ReleaseStringUTFChars(env, (jstring)result, str_result);
//    }
//    // Check if result is a Number
//    else if ((*env)->IsInstanceOf(env, result, number_class)) {
//        jmethodID double_value = (*env)->GetMethodID(env, number_class, "doubleValue", "()D");
//        jdouble num_result = (*env)->CallDoubleMethod(env, result, double_value);
//        lua_pushnumber(L, num_result);
//    }
//    // Check if result is a Boolean
//    else if ((*env)->IsInstanceOf(env, result, boolean_class)) {
//        jmethodID boolean_value = (*env)->GetMethodID(env, boolean_class, "booleanValue", "()Z");
//        jboolean bool_result = (*env)->CallBooleanMethod(env, result, boolean_value);
//        lua_pushboolean(L, bool_result);
//    }
//    // Fallback to toString
//    else {
//        jstring jstr = (*env)->CallObjectMethod(env, result, (*env)->GetMethodID(env, (*env)->GetObjectClass(env, result), "toString", "()Ljava/lang/String;"));
//        const char *str_result = (*env)->GetStringUTFChars(env, jstr, NULL);
//        lua_pushstring(L, str_result);
//        (*env)->ReleaseStringUTFChars(env, jstr, str_result);
//        (*env)->DeleteLocalRef(env, jstr);
//    }
//
//    // Clean up class references and result
//    (*env)->DeleteLocalRef(env, string_class);
//    (*env)->DeleteLocalRef(env, number_class);
//    (*env)->DeleteLocalRef(env, boolean_class);
//    (*env)->DeleteLocalRef(env, result);
//    return 1;
//}

