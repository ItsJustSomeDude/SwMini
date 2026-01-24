#include "mini.h"
#include "log.h"
#include "caver/component.h"

#define LOG_TAG "MiniRecreateHero"

typedef struct {
	float x;
	float y;
	float z;
} Vector3;

STATIC_DL_FUNCTION_SYMBOL(
	EntityCompnent__Interface,
	"_ZN5Caver15EntityComponent9InterfaceEv",
	void*, ()
)

STATIC_DL_FUNCTION_SYMBOL(
	GameSceneController__CreateHeroObjectAt,
	"_ZN5Caver19GameSceneController18CreateHeroObjectAtERKNS_7Vector3Eib",
	void, (void *this, Vector3 *location, int facing_direction, bool addToScene)
)

// Most of this logic comes from an if block in GameSceneController::EquipItem.
// It's the block with the CreateHeroObjectAt call.
int recreate_hero(lua_State *L) {
	// Get the gameController on the top of the stack.
	lua_getglobal(L, "gameController");
	if (!lua_islightuserdata(L, -1)) {
		LOGE("Could not find gameController!");
		luaL_error(L, "Could not find gameController!");
		return 1;
	}

	const void *gameController = lua_topointer(L, -1);
	LOGD("Found GameViewController: %p", gameController);

	// To find GameSceneController, go to GameViewController::GameControlButtonUp, and look for the
	// call to GameSceneController::ConsumeItem. The first param is the offset of gameSceneController.
	void *game_scene_controller = *$(void*, gameController, 0x68, 0xc8);
	LOGD("Found GameSceneController: %p", game_scene_controller);

	// To find this offset, look at the beginning of the if block we're copying.
	void *hero = *$(void*, game_scene_controller, 0xa4, 0xd8);
	LOGD("Found Hero: %p", hero);

//	LOGD(
//		"Trying to read positions... %f %f %f",
//		$(float, hero, 0, 0x70),
//		$(float, hero, 0, 0x74),
//		$(float, hero, 0, 0x78)
//	);

	// The first thing after fetching hero in the if block is the position Vector3.
	Vector3 *pos = $(Vector3, hero, 0x40, 0x70);

	// This offset is in the CreateHeroObjectAt call.
	void *component = ComponentWithInterface(hero, EntityCompnent__Interface);
	int current_direction = *$(int, component, 0x3c, 0x70);
	LOGD("Direction? %i", current_direction);

	GameSceneController__CreateHeroObjectAt(
		game_scene_controller,
		pos,
		current_direction,
		false
	);

	return 0;
}

void init_lua_recreate_hero() {
	dlsym_EntityCompnent__Interface();
	dlsym_GameSceneController__CreateHeroObjectAt();
}
