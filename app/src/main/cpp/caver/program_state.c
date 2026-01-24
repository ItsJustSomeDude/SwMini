#include "program_state.h"

#include "hooks.h"

DL_FUNCTION_SYMBOL(
	program_state_from_L,
	"_ZN5Caver12ProgramState12FromLuaStateEP9lua_State",
	void*, (lua_State * L)
)

void init_caver_program_state() {
	dlsym_program_state_from_L();
}

