#ifndef NEEDLE_PROGRAM_STATE_H
#define NEEDLE_PROGRAM_STATE_H

#include "hooks.h"
#include "lua.h"

H_DL_FUNCTION(
	program_state_from_L,
	void*, (lua_State * L)
)

#endif //NEEDLE_PROGRAM_STATE_H
