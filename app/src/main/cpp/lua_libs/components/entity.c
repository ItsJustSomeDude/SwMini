#include "caver/components.h"
#include "core/log.h"
#include "lua.h"
#include "lauxlib.h"
#include "components.h"
#include "helpers.h"

#define LOG_TAG "MiniEntityComponent"

SIMPLE_COMPONENT_FUNC(speed_cap, float, 0x68, 0xac, luaL_checknumber, lua_pushnumber)

SIMPLE_COMPONENT_FUNC(gravity, float, 0x68, 0xac, luaL_checknumber, lua_pushnumber)

const ComponentField EntityFields[] = {
	{"SpeedCap", speed_cap},
	{"Gravity",  gravity},

	{NULL, NULL}
};