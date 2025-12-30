#include "../hooks.h"
#include "lua.h"
#include "../log.h"
#include "lauxlib.h"

#define LOG_TAG "MiniLuaFindAll"

STATIC_DL_FUNCTION_OFFSET(mapIterHelper, 0x54b5d0, void*, (void* arg))

int scene_find_all(lua_State *L) {
	// Get the scene on the top of the stack.
	lua_getglobal(L, "scene");
	if (!lua_islightuserdata(L, -1)) {
		LOGE("Could not find scene!");
		luaL_error(L, "Could not find scene pointer!");
		return 1;
	}

	const void *scene = lua_touserdata(L, -1);
	LOGD("Found Scene: %p", scene);

	// The thing at c8 is the start of the Loop.
	long *scene_c8 = queryOffset(long*, scene, 0, 0xc8);
	// Not 100% sure...
	long objTree = queryOffset(int, scene, 0, 0xb8);

	dlsym_mapIterHelper();
	LOGD("Fetched iter helper: %p", mapIterHelper);
	LOGD("Ok, first fetching the data I need from scene... %p %p", scene_c8, objTree);

	// Create the Output table we will be all the SceneObjects in.
	lua_newtable(L); // -0, +1

	int i = 0;
	for (void *so = scene_c8; &queryOffset(void*, scene, 0, 0xb8) != so; so = mapIterHelper(so)) {
		i++;    // Lua starts at 1... wow.
		LOGD("num: %d, SceneObject pointer: %p", i, so);

		// Create the Lua representation of SceneObject.
		// TODO: Figure out if this needs to be 0x4 on 32-bit
		// Real type: SceneObject**
		void **object = lua_newuserdata(L, sizeof(void *)); // -0, +1
		// Grab the metatable from the registry.
		lua_getfield(L, LUA_REGISTRYINDEX, "SceneObject"); // -0, +1
		// Set it on the SceneObject UD
		lua_setmetatable(L, -2); // -1, +0
		// Place the fetched Instance into the UserData.
		*object = so;

		// Stack at this point: UD is on the top, Output Table is below it.

		lua_rawseti(L, -2, i); // -1, +0

		// Output table should be on top of the stack again.
	}

	// Still on top of stack, return it.
	return 1;
}

void init_scene_find_all() {
	dlsym_mapIterHelper();
}
