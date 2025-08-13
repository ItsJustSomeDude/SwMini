//#include "jni.h"
//#include "log.h"
//
//#include <jni.h>
//#include <string.h>
//#include <pthread.h>
//
//#define LOG_TAG "MiniJNI"
//
//// Global variables for JNI environment and JVM
//static JavaVM* g_jvm = NULL;
//static pthread_key_t thread_key;
//static pthread_once_t key_once = PTHREAD_ONCE_INIT;
//static jclass g_nativeBridge = NULL; // Global reference to NativeBridge class
//
//static jmethodID g_processInJava = NULL; // Method ID for processInJava
//
//// Cleanup function for thread-specific JNI environment
//static void detach_thread(void* env) {
//    if (g_jvm && env) {
//        (*g_jvm)->DetachCurrentThread(g_jvm);
//    }
//}
//
//// Create thread-specific key for JNI environment
//static void create_thread_key() {
//    pthread_key_create(&thread_key, detach_thread);
//}
//
//// Get or attach JNI environment for the current thread
//JNIEnv* get_jni_env() {
//    JNIEnv* env = NULL;
//    if (!g_jvm) {
//        return NULL;
//    }
//
//    // Check if thread is already attached
//    jint result = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
//    if (result == JNI_EDETACHED) {
//        // Attach thread and store env in thread-specific storage
//        result = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
//        if (result == JNI_OK) {
//            pthread_setspecific(thread_key, env);
//        } else {
//            return NULL;
//        }
//    } else if (result != JNI_OK) {
//        return NULL;
//    }
//    return env;
//}
//
//JNIEXPORT void JNICALL Java_net_itsjustsomedude_swrdg_NativeBridge_init(JNIEnv *env, jclass clazz) {
//    // Store JVM reference and initialize thread key
//    (*env)->GetJavaVM(env, &g_jvm);
//    pthread_once(&key_once, create_thread_key);
//
//    // Store global reference to the class
//    g_nativeBridge = (*env)->NewGlobalRef(env, clazz);
//
//    g_processInJava = (*env)->GetStaticMethodID(env, g_nativeBridge, "processInJava", "(Ljava/lang/String;)Ljava/lang/String;");
//    if (!g_processInJava) LOGE("Failed to find method: %s", "processInJava");
//
//
//}
//
//JNIEXPORT void JNICALL Java_com_example_myapp_NativeBridge_processData(JNIEnv *env, jclass clazz, jint value) {
//    JNIEnv* thread_env = get_jni_env();
//    if (!thread_env) {
//        return;
//    }
//    // Example processing
//    // Add your performance-critical processing here
//}
//
//JNIEXPORT jstring JNICALL Java_com_example_myapp_NativeBridge_getProcessedString(JNIEnv *env, jclass clazz, jstring input) {
//    JNIEnv* thread_env = get_jni_env();
//    if (!thread_env) {
//        return NULL;
//    }
//
//    // Convert jstring to const char*
//    const char* input_str = (*thread_env)->GetStringUTFChars(thread_env, input, NULL);
//    if (!input_str) {
//        return NULL;
//    }
//
//    // Process string (example: concatenate with prefix)
//    char output_str[256];
//    snprintf(output_str, sizeof(output_str), "Processed: %s", input_str);
//
//    // Release input string
//    (*thread_env)->ReleaseStringUTFChars(thread_env, input, input_str);
//
//    // Convert result back to jstring
//    jstring result = (*thread_env)->NewStringUTF(thread_env, output_str);
//    return result;
//}
//
//JNIEXPORT jstring JNICALL Java_com_example_myapp_NativeBridge_passStringToJavaAndBack(JNIEnv *env, jclass clazz, jstring input) {
//    JNIEnv* thread_env = get_jni_env();
//    if (!thread_env) {
//        return NULL;
//    }
//
//    // Convert jstring to const char* for C processing
//    const char* input_str = (*thread_env)->GetStringUTFChars(thread_env, input, NULL);
//    if (!input_str) {
//        return NULL;
//    }
//
//    // Process in C (example: add a prefix)
//    char c_processed[256];
//    snprintf(c_processed, sizeof(c_processed), "C: %s", input_str);
//    (*thread_env)->ReleaseStringUTFChars(thread_env, input, input_str);
//
//    // Convert C string to jstring for Java call
//    jstring java_input = (*thread_env)->NewStringUTF(thread_env, c_processed);
//    if (!java_input) {
//        return NULL;
//    }
//
//    // Call Java method processInJava
//    jstring java_result = (*thread_env)->CallStaticObjectMethod(thread_env, g_nativeBridge, g_processInJava, java_input);
//    (*thread_env)->DeleteLocalRef(thread_env, java_input);
//
//    // Check for exceptions
//    if ((*thread_env)->ExceptionCheck(thread_env)) {
//        (*thread_env)->ExceptionClear(thread_env);
//        return NULL;
//    }
//
//    return java_result;
//}