#include "features/cstrings/cstrings.h"
#include "core/log.h"
#include "mini.h"
#include "core/cpp_strings.h"

#include <stdlib.h>
#include <time.h>

#define LOG_TAG "map"

#define ARR_COUNT(array) (sizeof(array) / sizeof(array[0]))

const char *nothing_to_see_here[] = {
	"Nothing to see here.",
	"Move along now.",
	"The map function has no other behavior. Trust.",
	"Trust.",
	"It's definitely a user error...",
	"The map function has no secrets.",
	"Don't worry about it.",
	"Try again, but nothing cool will happen."
};

#define pick_one(array) \
    array[random() % ARR_COUNT(array)]

static int table_map(lua_State *L) {
	lua_settop(L, 2); /* Ensure the stack is clean. */
	lua_createtable(L, (int) lua_objlen(L, 1), 0); /* Output Table on -1 and 3 of stack. */

	lua_pushnil(L);  /* push first key: -1 and 4 */
	while (lua_next(L, 1) != 0) {
		/* 'key' at index -2 and 4; 'value' at index -1 and 5 */

		/* callback(value, key, array) */
		lua_pushvalue(L, 2); /* callback */
		lua_pushvalue(L, 5); /* value */
		lua_pushvalue(L, 4); /* key */
		lua_pushvalue(L, 1); /* array */
		lua_call(L, 3, 1); /* 3 args, 1 result */
		/* Stack is now back to before call, but with result in -1 and 6:
		 * 1 / -6 = input table
		 * 2 / -5 = input callback
		 * 3 / -4 = output table
		 * 4 / -3 = current key
		 * 5 / -2 = current value
		 * 6 / -1 = result
		 */

		/* duplicate the key because rawset will consume it, overwrite callback input. */
		lua_pushvalue(L, 4);
		lua_replace(L, 5);

		/* output[key] = result */
		lua_rawset(L, 3);

		/* Key is now top of stack for next `next`. */
	}
	/* Output is on top. */
	return 1;
}

static int array_map(lua_State *L) {
	lua_settop(L, 2); /* Ensure the stack is clean. */
	/* Remember, Callback == 1, Array == 2! */
	size_t len = lua_objlen(L, 2);

	/* create output table with array size */
	lua_createtable(L, (int) len, 0); /* output table at index 3 */

	for (int i = 1; i <= len; i++) {
		/* callback(value, index, array) */
		lua_pushvalue(L, 1);     /* callback */
		lua_rawgeti(L, 2, i);    /* get input[i] */
		lua_pushinteger(L, i);   /* index */
		lua_pushvalue(L, 2);     /* array */

		lua_call(L, 3, 1);       /* result */

		/* output[i] = result */
		lua_rawseti(L, 3, i);
	}

	return 1;
}

static int string_map(lua_State *L) {
	lua_settop(L, 2); /* Ensure the stack is clean. */
	size_t len;
	const char *str = lua_tolstring(L, 1, &len);

	lua_createtable(L, (int) len, 0);

	for (int i = 0; i < len; i++) {
		char c = str[i];

		lua_pushvalue(L, 2); /* callback */
		lua_pushlstring(L, &c, 1); /* Current char */
		lua_pushinteger(L, i + 1); /* Current index into string */
		lua_pushvalue(L, 1); /* whole string */
		lua_call(L, 3, 1); /* 3 args, 1 result */

		lua_rawseti(L, 3, i + 1);
	}

	lua_getglobal(L, "table");        // ... table table
	lua_getfield(L, -1, "concat");    // ... table table concat
	lua_remove(L, -2);                // remove "table" table -> ... table concat

	lua_pushvalue(L, -2);       // push original table
	lua_call(L, 1, 1);

	return 1;
}

static int number_map(lua_State *L) {
	lua_settop(L, 2); /* Ensure the stack is clean. */
	lua_Integer count = lua_tointeger(L, 1);

	lua_createtable(L, (int) count, 0); /* 3, -3 */

	for (int i = 1; i < count; i++) {
		/* callback(index, table) */
		lua_pushvalue(L, 2); /* callback */
		lua_pushinteger(L, i); /* index */
		lua_pushvalue(L, 3); /* table */
		lua_call(L, 2, 2);

		if (lua_isnil(L, -1)) {
			/* Function returned one thing. */
			/* output[i] = return_val */
			lua_pop(L, 1); /* Remove the nil. */
			lua_rawseti(L, 3, i);
		} else {
			/* Function returned two things. */
			/* output[top] = return_val OR 3[-1] = -2 */
			lua_insert(L, -2); /* Swap -2 and -1 */
			lua_settable(L, 3);
		}
	}
	return 1;
}

int miniLL_the_map_function(lua_State *L) {
	int num_args = lua_gettop(L);

	if (num_args == 0) {
		/* map() */
		return luaL_error(L, "Argument(s) expected.");

	} else if (num_args == 2 && lua_istable(L, 1) && lua_isfunction(L, 2)) {
		return table_map(L);

	} else if (num_args == 2 && lua_isfunction(L, 1) && lua_istable(L, 2)) {
		return array_map(L);

	} else if (num_args == 2 && lua_type(L, 1) == LUA_TSTRING && lua_isfunction(L, 2)) {
		return string_map(L);

	} else if (num_args == 2 && lua_isnumber(L, 1) && lua_isfunction(L, 2)) {
		return number_map(L);

	}

	/* The map function would not like to reveal her secrets. */

	return luaL_error(L, "Invalid argument(s)! (%s)", pick_one(nothing_to_see_here));
}

void initLL_map_function() {
}