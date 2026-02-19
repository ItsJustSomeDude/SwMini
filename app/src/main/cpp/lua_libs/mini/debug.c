/* clang-format off */
#include "mini.h" /* clang-format on */

#include "hooks.h"
#include "log.h"
#include "lua.h"

#define LOG_TAG "MiniLuaDebug"

STATIC_DL_FUNCTION_SYMBOL(
	toggle_debug_info,
	"_ZN5Caver13GameSceneView15ToggleDebugInfoEv",
	void, (void* this)
)

int tdb(lua_State *L) {
	lua_getglobal(L, "gameController");

	const void *gameController = lua_topointer(L, -1);
	LOGD("Found GameViewController: %p", gameController);

	// Offset found from GameViewController::LevelUpViewControllerDidFinish:
	// ExperienceBar::UpdateExperience(this->sceneView->overlayView->expBar);
	//                                 ^     ^          ^
	//                                  \-GameViewController
	//                                        \-GameSceneView
	//                                                   \-GameOverlayView

	void *gameSceneView = *$(void*, gameController, 0x70, 0xd8);
	LOGD("GameSceneView: %p", gameSceneView);

	toggle_debug_info(gameSceneView);

	void *debug_overlay = *$(void*, gameSceneView, 0xd4, 0x110);

	*$(int, debug_overlay, 0x10c, 0x164) = 1;

	LOGD("Enabled debug overlay maybe. %p", debug_overlay);


	return 0;
}

STATIC_DL_HOOK_SYMBOL(
	overlay_toggle,
	"_ZN5Caver16DebugInfoOverlay6ToggleEv",
	void, (void* this)
) {
	LOGD("The actual toggle function WAS called, this = %p", this);

	orig_overlay_toggle(this);

	LOGD("Done.");
}

STATIC_DL_HOOK_SYMBOL(
	overlay_update,
	"_ZN5Caver16DebugInfoOverlay6UpdateEf",
	void, (void* this, float delta)
) {
//	LOGD("Update called successfully: %p %f", this, delta);
	orig_overlay_update(this, delta);
}

STATIC_DL_FUNCTION_SYMBOL(
	overlay_update,
	"_ZN5Caver16DebugInfoOverlay6UpdateEf",
	void, (void* this, float delta)
)


void init_lua_debug() {
	dlsym_toggle_debug_info();

	hook_overlay_toggle();
//	hook_overlay_update();
	dlsym_overlay_update();
}
