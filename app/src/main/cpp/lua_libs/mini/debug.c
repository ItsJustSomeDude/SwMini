
#include "hooks.h"
#include "lua.h"
#include "log.h"

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

	LOGD("Enabled debug overlay maybe.");

	return 0;
}

void init_lua_debug() {
	dlsym_toggle_debug_info();
}
