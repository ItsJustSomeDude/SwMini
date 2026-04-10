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

