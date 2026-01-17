#include "lni.h"
#include "lua.h"
#include "lauxlib.h"
#include "log.h"

#define LOG_TAG "MiniLNIBind"

// TODO: Also push ExecuteLNI as an upvalue instead of fetching at bind run time.

static int bind_inner(lua_State *L) {
	int nargs = lua_gettop(L);

	// Get upvalue[1] = function name string
	const char *name = lua_tostring(L, lua_upvalueindex(1));

	// Push _G.Mini.ExecuteLNI
	lua_getglobal(L, "Mini");                 // push _G.Mini table
	lua_getfield(L, -1, "ExecuteLNI"); // push the function
	lua_remove(L, -2);                    // remove Mini table

	// First argument: the bound name
	lua_pushstring(L, name);

	// Forward all passed arguments
	for (int i = 1; i <= nargs; ++i)
		lua_pushvalue(L, i);

	lua_call(L, nargs + 1, LUA_MULTRET);

	// Get number of values returned from function.
	int ret = lua_gettop(L) - nargs;

	return ret;

//	LOGD("Stack height after call and return: %i", );
//
//	return lua_gettop(L);


//	// Use pcall to safely forward multiple returns
//	if (lua_pcall(L, nargs + 1, LUA_MULTRET, 0) != 0) {
//		// On error, propagate it (original error message is on top)
//		return lua_error(L);
//	}
//
//
//
//	// Return all results from ExecuteLNI
//	return lua_gettop(L);
}

int lni_bind(lua_State *L) {
	const char *name = luaL_checkstring(L, 1);

	// Store the name as upvalue
	lua_pushstring(L, name);

	lua_pushcclosure(L, &bind_inner, 1);

	return 1;
}

void lni_bind_global(lua_State *L, const char *func, int count, ...) {
	lua_getglobal(L, "LNI");
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);      // Remove nil
		lua_newtable(L);    // Create a table
		lua_pushvalue(L, -1);   // Duplicate the reference on the stack
		lua_setglobal(L, "LNI");    // Make it global
		// It's still on top of the stack.
	}

	// Store the name as upvalue
	lua_pushstring(L, func);
	// Push bound function
	lua_pushcclosure(L, &bind_inner, 1);

	// Stack: ..., -2: LNI table, -1: closure

	va_list args;
	va_start(args, count);
	for (int i = 0; i < count; i++) {
		const char *name = va_arg(args, const char*);
//		LOGD("Setting %i name %s", i, name);
		if (i < count - 1) {
			// There's another name to set, so duplicate the closure first.
			lua_pushvalue(L, -1);
			// Stack: ..., -3: LNI table, -2: closure, -1: closure
			lua_setfield(L, -3, name);
			// Stack: ..., -2: LNI table, -1: closure
		} else {
			// Last one, consume the closure.
			lua_setfield(L, -2, name);
		}
	}
	va_end(args);

	// Remove LNI table.
	lua_pop(L, 1);
}
