#include "../../hooks.h"
#include "lua.h"
#include "../../log.h"
#include "lauxlib.h"
#include "../../iterate.h"

#define LOG_TAG "MiniLuaFindAll"

STATIC_DL_FUNCTION_OFFSET(mapIterHelper, 0x54c640, void*, (void* arg))

STATIC_DL_FUNCTION_SYMBOL(
	pushSceneObject,
	"_ZN5Caver14SceneObjectLib15PushSceneObjectEPNS_12ProgramStateEPNS_11SceneObjectE",
	void, (void *programState, void *sceneObject)
)

STATIC_DL_FUNCTION_SYMBOL(
	progStateFromL,
	"_ZN5Caver12ProgramState12FromLuaStateEP9lua_State",
	void*, (lua_State * L)
)

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

	void *objTreeMaybe = $(void*, scene, 0, 0xb0);

	LOGD("Iterating the tree...");
	//
	iterate_map(&objTreeMaybe);



//	// Not 100% sure...
//	void *objTree = $(void*, scene, 0, 0xb8);
//
//	void *progState = progStateFromL(L);
//
//	LOGD("Fetched iter helper: %p", mapIterHelper);
//	LOGD("Ok, first fetching the data I need from scene... %p", objTree);
//
//	// Create the Output table we will be all the SceneObjects in.
//	lua_newtable(L); // -0, +1
//
//	int i = 0;
//	for (
//		void *so = $(long*, scene, 0, 0xc8);
//		so != &$(void*, scene, 0, 0xb8);
//		so = mapIterHelper(so)
//		) {
//
////		void *so10 = *(void **) (so + 10);
//
//		i++;    // Lua starts at 1... wow.
//		LOGD("num: %d, SceneObject pointer: %p", i, so);
//
//		if (&$(void*, scene, 0, 0xb8) == so) {
//			LOGD("Skipping because AnotherTree?");
//			continue;
//		}
//
//		void *obj_some_pointer = $(void*, so, 0, 0x22);
//
//		// I have no idea what I'm doing :-)
//		void *objInner = *(&obj_some_pointer + 6);
//
//		if (objInner == NULL) {
//			LOGD("Something I don't understand is null");
//			continue;
//		}
//
//		LOGD("More stuff, %p %p", obj_some_pointer, objInner);
//		LOGD("This _should_ be the usage count of the object: %li", $(long, objInner, 0, 0x8));
//
//		pushSceneObject(progState, objInner);
//
//		void **luaObj = lua_touserdata(L, -1);
//
//		LOGD("UD = %p, *UD = %p", luaObj, *luaObj);
//
//		// I think it's on the top of the stack???
//		// I hate side effects...
//
//		lua_rawseti(L, -2, i);
//
//
//
//
//
////		// Create the Lua representation of SceneObject.
////		// TODO: Figure out if this needs to be 0x4 on 32-bit
////		// Real type: SceneObject**
////		void **object = lua_newuserdata(L, sizeof(void *)); // -0, +1
////		// Grab the metatable from the registry.
////		lua_getfield(L, LUA_REGISTRYINDEX, "SceneObject"); // -0, +1
////		// Set it on the SceneObject UD
////		lua_setmetatable(L, -2); // -1, +0
////		// Place the fetched Instance into the UserData.
////		*object = so;
//
//		// Stack at this point: UD is on the top, Output Table is below it.
//
////		lua_rawseti(L, -2, i); // -1, +0
//
//		// Output table should be on top of the stack again.
//	}
//
//	// Still on top of stack, return it.
//	return 1;
	return 0;
}

void init_scene_find_all() {
	dlsym_mapIterHelper();
	dlsym_pushSceneObject();
	dlsym_progStateFromL();
}
