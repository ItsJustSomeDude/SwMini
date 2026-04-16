#include "caver/components.h"
#include "caver/scene_object.h"
#include "core/hooks.h"
#include "lauxlib.h"
#include "core/log.h"
#include "lua.h"

#define LOG_TAG "MiniCharAttrib"

static SceneObject *get_hero(lua_State *L) {
	lua_getglobal(L, "gameController");
	if (!lua_islightuserdata(L, -1)) {
		LOGE("Could not find gameController!");
		return NULL;
	}
	void *gameController = lua_touserdata(L, -1);
	void *game_scene_controller = *$(void*, gameController, 0x68, 0xc8);
	if (!game_scene_controller) {
		LOGE("GameSceneController is null");
		return NULL;
	}
	void *hero = *$(void*, game_scene_controller, 0xa4, 0xd8);
	if (!hero) {
		LOGE("Hero is null");
		return NULL;
	}
	return (SceneObject *) hero;
}

#define ATTRIBUTE_FUNCTIONS(NAME, TYPE, OFF32, OFF64, LUA_GET, LUA_PUSH) \
static int get_##NAME(lua_State *L) { \
    void *hero = get_hero(L); \
    if (!hero) return 0; \
    void *component = SceneObject_ComponentWithInterface(hero, CharControllerComponent_Interface); \
    if (!component) return 0; \
    TYPE value = *$(TYPE, component, OFF32, OFF64); \
    LUA_PUSH(L, value); \
    return 1; \
} \
static int set_##NAME(lua_State *L) { \
    void *hero = get_hero(L); \
    if (!hero) return 0; \
    TYPE new_value = (TYPE)LUA_GET(L, 1); \
    void *component = SceneObject_ComponentWithInterface(hero, CharControllerComponent_Interface); \
    if (!component) return 0; \
    *$(TYPE, component, OFF32, OFF64) = new_value; \
    return 0; \
}

ATTRIBUTE_FUNCTIONS(walk_speed, float, 0x170, 0x278, luaL_checknumber, lua_pushnumber)

ATTRIBUTE_FUNCTIONS(run_speed, float, 0x178, 0x280, luaL_checknumber, lua_pushnumber)

ATTRIBUTE_FUNCTIONS(jump_height, float, 0x164, 0x26c, luaL_checknumber, lua_pushnumber)

ATTRIBUTE_FUNCTIONS(air_jump_used, int, 0x158, 0x260, lua_toboolean, lua_pushboolean)

const luaL_Reg attrib_functions[] = {
	{"GetWalkSpeed",   get_walk_speed},
	{"SetWalkSpeed",   set_walk_speed},

	{"GetRunSpeed",    get_run_speed},
	{"SetRunSpeed",    set_run_speed},

	{"GetJumpHeight",  get_jump_height},
	{"SetJumpHeight",  set_jump_height},

	{"GetAirJumpUsed", get_air_jump_used},
	{"SetAirJumpUsed", set_air_jump_used},

	{NULL, NULL}
};
