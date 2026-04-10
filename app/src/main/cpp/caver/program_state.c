#include "program_state.h"

#include "init/engine.h"
#include "core/hooks.h"
#include "core/log.h"
#include "lauxlib.h"

#define LOG_TAG "MiniProgramState"

DL_FUNCTION_SYMBOL(
	program_state_from_L,
	"_ZN5Caver12ProgramState12FromLuaStateEP9lua_State",
	void*, (lua_State * L)
)

void initE_program_state() {
	dlsym_program_state_from_L();
}

void *game_controller_from_L(lua_State *L) {
	lua_getglobal(L, "gameController");
	if (!lua_islightuserdata(L, -1)) {
		LOGE("Could not find gameController!");
		luaL_error(L, "Could not find gameController!");
		return NULL;
	}
	void *c = lua_touserdata(L, -1);
	lua_pop(L, 1);
	return c;
}

void *scene_controller_from_L(lua_State *L) {
	void *game_controller = game_controller_from_L(L);

	void *scene_controller = *$(void*, game_controller, 0x68, 0xc8);
	if (scene_controller == NULL) {
		LOGE("Could not find GameSceneController!");
		luaL_error(L, "Could not find GameSceneController!");
		return NULL;
	}
	return scene_controller;
}

SceneObject *hero_object_from_L(lua_State *L) {
	void *scene_controller = scene_controller_from_L(L);
	SceneObject *hero = *$(SceneObject*, scene_controller, 0x4, 0x8);
	if (hero == NULL) {
		LOGE("Unable to find Hero object!");
		luaL_error(L, "Unable to find Hero object!");
		return NULL;
	}
	return hero;
}
