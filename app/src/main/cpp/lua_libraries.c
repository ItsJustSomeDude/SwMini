#include <jni.h>
#include <string.h>

#include "lua_libraries.h"
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "symbol.h"
#include "caver.h"
#include "luasocket/src/luasocket.h"
#include "log.h"
#include "lni2.h"
#include "lua_ext/lfs.h"

#define LOG_TAG "MiniLuaLibs"

static int setControlsHidden (lua_State *L) {
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    int hidden = lua_toboolean(L, 1);

    // Get the gameController on the top of the stack.
    lua_getglobal(L, "gameController");
    if (!lua_islightuserdata(L, -1)) {
        LOGD("Could not find gameController!");
        luaL_error(L, "Could not find gameController!");
        return 1;
    }

    const void* gameController = lua_topointer(L, -1);
    LOGD("Found GameViewController: %p", gameController);

    // Offset found from GameViewController::LevelUpViewControllerDidFinish:
    // ExperienceBar::UpdateExperience(this->sceneView->overlayView->expBar);
    //                                 ^     ^          ^
    //                                  \-GameViewController
    //                                        \-GameSceneView
    //                                                   \-GameOverlayView

    void* gameSceneView = $(void*, gameController, 0x70, 0xd8);
    LOGD("GameSceneView: %p", gameSceneView);

    // These next two offsets can be found in Caver::GameSceneView::SetCinematicModeEnabled
    // GameOverlayView::CancelInput(this->field201_0xcc);
    // this->field201_0xcc->field188_0xbc = true;
    //       ^              ^
    // 64bit:0x100          0xe4

    void* gameOverlayView = $(void*, gameSceneView, 0xcc, 0x100);
    LOGD("GameOverlayView: %p", gameOverlayView);

    $(bool, gameOverlayView, 0xbc, 0xe4) = hidden;
    GameOverlayView_SetControlsHidden(gameOverlayView, hidden);

    return 0;
}

STATIC_DL_FUNCTION_OFFSET(mapIterHelper, 0x54b5d0, void*, (void* arg))

static int test2 (lua_State *L) {
    // Get the scene on the top of the stack.
    lua_getglobal(L, "scene");
    if (!lua_islightuserdata(L, -1)) {
        LOGE("Could not find scene!");
        luaL_error(L, "Could not find scene pointer!");
        return 1;
    }

    const void* scene = lua_touserdata(L, -1);
    LOGD("Found Scene: %p", scene);

    // The thing at c8 is the start of the Loop.
    long* scene_c8 = queryOffset(long*, scene, 0, 0xc8);
    // Not 100% sure...
    long objTree = queryOffset(int, scene, 0, 0xb8);

    dlsym_mapIterHelper();
    LOGD("Fetched iter helper: %p", mapIterHelper);
    LOGD("Ok, first fetching the data I need from scene... %p %p", scene_c8, objTree);

    // Create the Output table we will be all the SceneObjects in.
    lua_newtable(L); // -0, +1

    int i = 0;
    for (void* so = scene_c8; &queryOffset(void*, scene, 0, 0xb8) != so; so = mapIterHelper(so)) {
        i++;    // Lua starts at 1... wow.
        LOGD("num: %d, SceneObject pointer: %p", i, so);

        // Create the Lua representation of SceneObject.
        // TODO: Figure out if this needs to be 0x4 on 32-bit
        // Real type: SceneObject**
        void** object = lua_newuserdata(L, sizeof(void*)); // -0, +1
        // Grab the metatable from the registry.
        lua_getfield(L,LUA_REGISTRYINDEX,"SceneObject"); // -0, +1
        // Set it on the SceneObject UD
        lua_setmetatable(L,-2); // -1, +0
        // Place the fetched Instance into the UserData.
        *object = so;

        // Stack at this point: UD is on the top, Output Table is below it.

        lua_rawseti(L, -2, i); // -1, +0

        // Output table should be on top of the stack again.
    }

    // Still on top of stack, return it.
    return 1;
}


static int getProfileID (lua_State *L) {
    if (latestProfileId == NULL) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushlstring(L, latestProfileId, strlen(latestProfileId));
    return 1;
}

static int getArch (lua_State *L) {
    lua_pushliteral(L, archSplit("armeabi-v7a", "arm64-v8a"));
    return 1;
}


#define MINI_MINILIBNAME "Mini"
static const luaL_Reg minilib[] = {
        {"SetControlsHidden", setControlsHidden},
        {"GetProfileID", getProfileID},
        {"Arch", getArch},
        {"ExecuteLNI", executeLNI},

//        { "TestGOV", test },
//        { "TestGAO", test2 },

        {NULL, NULL}
};

LUALIB_API int open_mini (lua_State *L) {
    luaL_register(L, MINI_MINILIBNAME, minilib);
    return 1;
}

STATIC_DL_HOOK_ADDR(openString, luaopen_string, void, (lua_State * L)) {
    LOGD("Loading all Patched Lua Libraries.");

    static const luaL_Reg lualibs[] = {
            // We cannot load base, it get loaded by the vanilla string bundle.
            // {"", luaopen_base},

            // Not adding package for now.
            // {LUA_LOADLIBNAME, luaopen_package},

            {LUA_TABLIBNAME, luaopen_table},
            {LUA_IOLIBNAME, luaopen_io},
            {LUA_OSLIBNAME, luaopen_os},

            // We cannot load string this way, because it's bundled in LS.
            // {LUA_STRLIBNAME, luaopen_string},

            {LUA_MATHLIBNAME, luaopen_math},
            {LUA_DBLIBNAME, luaopen_debug},

            // Mini!
            {MINI_MINILIBNAME, open_mini},

            {NULL, NULL}
    };

    const luaL_Reg *lib = lualibs;
    for (; lib->func; lib++) {
        lua_pushcfunction(L, lib->func);
        lua_pushstring(L, lib->name);
        lua_call(L, 1, 0);
    }

    luaopen_socket_core(L);
    lua_setglobal(L, "broken_socket");

    luaopen_lfs(L);

    orig_openString(L);

    // TODO: Possible Caver bug? An item is left on the stack, check for crashes. Mini pops it, but this might not be a good thing.
//    lua_pop(L, 1);
}

void setup_lua_lib_hook() {
    LOGD("Attempting to hook openString at addr %p", luaopen_string);

    hook_openString();
}
