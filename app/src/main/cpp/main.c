#include "main.h"

#include "log.h"
#include "java.h"
#include "libs/Gloss.h"

#include "hooks.h"

#include "patches/patches.h"
#include "mini_lua/library.h"

#define LOG_TAG "MiniNativeMain"

/** This is called by JNI_Onload, which is the earliest in the startup process.
 * libsw is not loaded yet. */
void earlyLoad() {
	LOGD("Performing Early-load");

	spawn_log_threads();

	cache_classes();

	GlossInit(false);

#ifdef NDK_DEBUG
	LOGD("GlossHook Logging Enabled!");
	GlossEnableLog(true);
#else
	GlossEnableLog(false);
#endif
}

/** This is called by Java after libsw is loaded, but before it's been told to initialize. */
void midLoad() {
	LOGD("Performing Mid-load");

	// This finds the reference and load bias for libsw.
	setup_hooks();

	// This sets up all the references for Lua's internals.
	setup_lua_core();

#ifdef __arm__
	setup32Patch();
#endif

//    setupNetworkHooks();

	setupAssetPatch();

	setup_panic_hook();
	setup_string_hook();

	init_mini_lua_lib();
	init_lua_libs();
}

/** This is called after libsw has been initialized from Java. */
void lateLoad() {
	LOGD("Performing Late-load");
}
