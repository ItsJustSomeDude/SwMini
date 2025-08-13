
#include <stddef.h>
#include <dlfcn.h>
#include <android/log.h>
#include <jni.h>
#include "string_hook.h"

#include "libs/khash.h"
#include "hook_core.h"
#include "libs/Gloss.h"
#include "symbol.h"
#include "log.h"

#define LOG_TAG "MiniCString"

KHASH_MAP_INIT_STR(str, char*)

static kh_str_t *h;
static khint_t k;

void addReplacement(const char* source, const char* replacement) {
    if(source == NULL || *source == '\0') return;

    const char* ourSource = strdup(source);

    int absent;
    k = kh_put_str(h, ourSource, &absent);
    if (absent) {
        LOGD("Added new string: '%s' -> '%s'", ourSource, replacement);
        kh_value(h, k) = strdup(replacement);
    }
}

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_NativeBridge_addStringReplacement(
        JNIEnv *env, jclass clazz, jstring jSource, jstring jReplacement
) {
    const char* source = (*env)->GetStringUTFChars(env, jSource, 0);
    const char* replacement = (*env)->GetStringUTFChars(env, jReplacement, 0);

    addReplacement(source, replacement);

    (*env)->ReleaseStringUTFChars(env, jSource, source);
    (*env)->ReleaseStringUTFChars(env, jReplacement, replacement);
}

void cleanup_string_hook() {
    // IMPORTANT: free memory allocated by strdup() above
    for (k = 0; k < kh_end(h); ++k)
        if (kh_exist(h, k))
            free((char*)kh_key(h, k));
    kh_destroy_str(h);
}

#define CREATE_BASIC_STRING_OFFSET splitOffset(0x37bbe0, 0x565b48)

DEFINE_SYMBOL_HOOK_H(createBasicString, void, (size_t **destinationPointer, char *strDataPointer))
DEFINE_HOOK_OFFSET(createBasicString, CREATE_BASIC_STRING_OFFSET, void, (size_t **destinationPointer, char *strDataPointer)) {
    if(strDataPointer == NULL || *strDataPointer == '\0') {
        orig_createBasicString(destinationPointer, strDataPointer);
        return;
    }

    k = kh_get_str(h, strDataPointer);
    if(k != kh_end(h)) {
        char *v = kh_value(h, k);
//        LOGD("Got string we have a hook for: %s -> %s", strDataPointer, v);
        orig_createBasicString(destinationPointer, v);
    } else {
        orig_createBasicString(destinationPointer, strDataPointer);
    }
}

void setup_string_hook() {
    h = kh_init_str();

//    addReplacement("Saved Games", "Hooks are working.");
//    addReplacement("Privacy Policy", "Open Mod Settings");

    hook_createBasicString();
}
