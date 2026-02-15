#include "main.h"

#include "log.h"
#include "java.h"
#include "libs/Gloss.h"

#include "hooks.h"

#include "patches/patches.h"
#include "lua_libs/mini/mini.h"
#include "features/coin_limit.h"
#include "features/cstrings/cstrings.h"
#include "caver/caver.h"
#include "features/armor_models.h"
#include "features/achievements/achievements.h"
#include "features/mini_config.h"

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
	init_lua();

	init_caver();

#ifdef __arm__
	init_patch_32bit();
#endif
	init_patch_assets();
	init_patch_panic();
	init_patch_lua_libs();
	// init_patch_network();

	init_mini_lua_lib();

	init_feature_cstrings();
	initF_coin_limit();
	initF_armor_models();
	init_feature_achievements();
	init_feature_state_tracking();

	read_mini_config_asset();
}

/** This is called after libsw has been initialized from Java. */
void lateLoad() {
	LOGD("Performing Late-load");
}
