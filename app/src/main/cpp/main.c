#include "init/main.h"

#include "config/mini_config.h"
#include "libs/Gloss.h"
#include "core/log.h"
#include "lua_libs/mini/mini.h"

#include "init/core.h"
#include "init/features.h"
#include "init/patches.h"
#include "init/engine.h"
#include "init/events.h"
#include "core/impl_files/resource_paths.h"
#include "init/lua_libs.h"

#include <stdbool.h>

#define LOG_TAG "MiniNativeMain"

#ifdef NDK_DEBUG

__attribute((constructor))
void earliest_load() {
	LOGD("LibMini has been loaded.");
}

#endif

/** This is called by JNI_Onload, which is fairly early in the startup process.
 * Engine Lib is not loaded yet. */
void earlyLoad() {
	LOGD("Performing Early-load");

	initC_logging();
	initC_jni();

	GlossInit(/* InitLinker: */ false);

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

	/* This sets up all the Mini <-> Engine bindings, needed for everything else. */
	initC_engine();

	/* Core Mini features */
	initC_lua();
	initC_cpp_strings();

	/* Engine Functions */
	initE_components();
	initE_program_state();

	/* Patches */
	initP_lua_panic();
	initP_program_state();
	initP_lua_loadfile();
	initP_external_assets();

#ifdef __arm__
	initP_program_32bit();
#endif /* __arm__ */

	/* Features */
	initF_cstrings();
	initF_armor_models();
	initF_achievements();

	/* Events */
	initEV_profile();

	/* Lua Libraries */
	initLL_set_controls_hidden();
	initLL_debug();
	initLL_recreate_hero();
	initLL_map_function();
	initLL_character();

	initLL_components();

	/* Misc Init stuff that needs to move eventually. */
	read_mini_config_asset();
}

/** This is called after libsw has been initialized from Java. */
void lateLoad() {
	LOGD("Performing Late-load");

	miniR_refresh_paths();
}
