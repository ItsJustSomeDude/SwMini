#include "lua.h"
#include "lauxlib.h"
#include "../log.h"
#include "../hooks.h"
#include "library.h"

#define LOG_TAG "MiniLuaControls"

STATIC_DL_FUNCTION_SYMBOL(
	GameOverlayView_SetControlsHidden,
	"_ZN5Caver15GameOverlayView17SetControlsHiddenEb",
	void, (void* this, bool hide)
)

int setControlsHidden(lua_State *L) {
	luaL_checktype(L, 1, LUA_TBOOLEAN);
	int hidden = lua_toboolean(L, 1);

	// Get the gameController on the top of the stack.
	lua_getglobal(L, "gameController");
	if (!lua_islightuserdata(L, -1)) {
		LOGE("Could not find gameController!");
		luaL_error(L, "Could not find gameController!");
		return 1;
	}

	const void *gameController = lua_topointer(L, -1);
	LOGD("Found GameViewController: %p", gameController);

	// Offset found from GameViewController::LevelUpViewControllerDidFinish:
	// ExperienceBar::UpdateExperience(this->sceneView->overlayView->expBar);
	//                                 ^     ^          ^
	//                                  \-GameViewController
	//                                        \-GameSceneView
	//                                                   \-GameOverlayView

	void *gameSceneView = $(void*, gameController, 0x70, 0xd8);
	LOGD("GameSceneView: %p", gameSceneView);

	// These next two offsets can be found in Caver::GameSceneView::SetCinematicModeEnabled
	// GameOverlayView::CancelInput(this->field201_0xcc);
	// this->field201_0xcc->field188_0xbc = true;
	//       ^              ^
	// 64bit:0x100          0xe4

	void *gameOverlayView = $(void*, gameSceneView, 0xcc, 0x100);
	LOGD("GameOverlayView: %p", gameOverlayView);

	$(bool, gameOverlayView, 0xbc, 0xe4) = hidden;
	GameOverlayView_SetControlsHidden(gameOverlayView, hidden);

	return 0;
}

void init_setControlsHidden() {
	dlsym_GameOverlayView_SetControlsHidden();
}
