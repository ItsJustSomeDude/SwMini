#ifndef NEEDLE_PROGRAM_STATE_H
#define NEEDLE_PROGRAM_STATE_H

#include "core/hooks.h"
#include "lua.h"
#include "scene_object.h"

H_DL_FUNCTION(
	program_state_from_L,
	void*, (lua_State * L)
)

#endif //NEEDLE_PROGRAM_STATE_H
