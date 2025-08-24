#include <stdio.h>
#include <string.h>
#include <android/log.h>
#include <jni.h>
#include <dlfcn.h>

#include "hook_core.h"
#include "hook.h"
#include "patches/cstrings.h"
#include "lua_core.h"
#include "lua_libraries.h"
#include "caver.h"
#include "libs/Gloss.h"
#include "patches/network.h"
#include "log.h"
#include "patches/32patch.h"
#include "patches/assets.h"

#define LOG_TAG "MiniHookCore"

static char* libSwPath;
static size_t libSwLength;
static void* libSwHandle;
static void* libSwBase;

void *hookSym(const char *symName, void *newAddr, void **origAddr) {
    void* sym = getSym(symName);
    if (sym == NULL) {
        LOGD("Failed to hook %s: Failed to find offset.", symName);
        return NULL;
    }

    return hookAddr(sym, newAddr, origAddr);
}

void *hookAddr(void* func_addr, void *new_addr, void **orig_addr) {
    void *stub = GlossHookAddr(
            func_addr,
            new_addr,
            orig_addr,
            false,
            archSplit(I_THUMB, I_ARM64)
    );
    if (stub == NULL) {
        LOGD("Failed to hook address: %p", func_addr);
        return NULL;
    }
    return stub;
}

void *hookOffset(unsigned long offset, void *new_addr, void **orig_addr) {
    return hookAddr(libSwBase + offset, new_addr, orig_addr);
}

void *getSym(const char* symbol) {
    void* ptr = dlsym(libSwHandle, symbol);
    if(ptr == NULL) {
        LOGD("Symbol '%s' lookup failed: %s", symbol, dlerror());
        return NULL;
    }
    return ptr;
}

void *getOffset(unsigned int offset) {
    return libSwBase + offset;
}

// Mid-loading - executed after lsw has been loaded into ram, but before init.
JNIEXPORT void JNICALL Java_net_itsjustsomedude_swrdg_NativeBridge_prepareHooks(JNIEnv* env, jobject obj) {
    libSwHandle = dlopen("libswordigo.so", RTLD_NOLOAD);
    libSwBase = (void*) GlossFindLibMapping("libswordigo.so", -1, libSwPath, &libSwLength);

#ifdef __arm__
    setup32Patch();
#endif

//    setupNetworkHooks();

    setupAssetPatch();

    setupCaverHooks();

    setup_string_hook();
    setup_lua_core();
    setup_lua_lib_hook();
    setupHooks();
}

JNIEXPORT void JNICALL Java_net_itsjustsomedude_swrdg_NativeBridge_cleanupHooks(JNIEnv* env, jobject obj) {
    cleanup_string_hook();
}