#include "caver/components.h"
#include "core/log.h"
#include "lua.h"
#include "lauxlib.h"
#include "components.h"
#include "helpers.h"

#define LOG_TAG "MiniPhysicsComponent"


SIMPLE_COMPONENT_FUNC(ground_deceleration, float, 0xc4, 0x10c, luaL_checknumber, lua_pushnumber)

SIMPLE_COMPONENT_FUNC(air_deceleration, float, 0xc8, 0x110, luaL_checknumber, lua_pushnumber)

SIMPLE_COMPONENT_FUNC(gravity, float, 0xa4, 0xe8, luaL_checknumber, lua_pushnumber)

SIMPLE_COMPONENT_FUNC(elasticity, float, 0xac, 0xf0, luaL_checknumber, lua_pushnumber)

const ComponentField PhysicsFields[] = {
	{"GroundDeceleration", ground_deceleration},
	{"AirDeceleration",    air_deceleration},
	{"Gravity",            gravity},
	{"Elasticity",         elasticity},

	{NULL, NULL}
};