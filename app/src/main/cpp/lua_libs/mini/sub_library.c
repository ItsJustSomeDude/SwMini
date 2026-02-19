/* clang-format off */
#include "sublib.h" /* clang-format on */

#include "lauxlib.h"
#include "lua.h"
#include "mini.h"

#define LOG_TAG "MiniSubLib"

void miniL_register(lua_State *L, const char *lib_name, const luaL_Reg *functions) {
	// Fetch or create Mini library table
	lua_getglobal(L, MINI_LIB_NAME);
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);      // Remove nil
		lua_newtable(L);    // Create a table
		lua_pushvalue(L, -1);   // Duplicate the reference on the stack
		lua_setglobal(L, MINI_LIB_NAME);    // Make it global
		// It's still on top of the stack.
	}

	// Mini.SubLibName to top of stack.
	lua_getfield(L, -1, lib_name);
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);      // Remove nil
		lua_newtable(L);    // Create a table
		lua_pushvalue(L, -1);   // Duplicate the reference on the stack
		// Stack is: -3: Mini, -2: Lib, -1: Lib
		lua_setfield(L, -3, lib_name);  // Store to Mini table.
		// Library is now top of stack.
	}

	// Remove Mini from stack.
	lua_remove(L, -2);
	// Library is on top.

	// Push all the functions to it.
	for (; functions->name; functions++) {
		lua_pushcclosure(L, functions->func, 0);
		lua_setfield(L, -2, functions->name);
	}

	lua_pop(L, 1);
}
