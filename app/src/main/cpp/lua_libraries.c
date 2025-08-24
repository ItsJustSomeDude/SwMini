#include <android/log.h>
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

    if (latestGameOverlayView == NULL) return 1;

    GameOverlayView_SetControlsHidden(latestGameOverlayView, hidden);
    *(bool *)(latestGameOverlayView + splitOffset(0, 0xe4)) = hidden;

    lua_pushnil(L);
    return 0;
}

static int getProfileID (lua_State *L) {
    // TODO: Push nil probably?
    if (latestProfileId == NULL) return 1;

    lua_pushlstring(L, latestProfileId, strlen(latestProfileId));
    return 1;
}

static int getArch (lua_State *L) {
    lua_pushliteral(L, splitLiteral("armeabi-v7a", "arm64-v8a"));
    return 1;
}


#define MINI_MINILIBNAME "Mini"
static const luaL_Reg minilib[] = {
        {"SetControlsHidden", setControlsHidden},
        {"GetProfileID", getProfileID},
        {"Arch", getArch},

        {NULL, NULL}
};

LUALIB_API int open_mini (lua_State *L) {
    luaL_register(L, MINI_MINILIBNAME, minilib);
    return 1;
}

DEFINE_SYMBOL_HOOK_H(openString, void, (lua_State* L))
DEFINE_HOOK_ADDR(openString, luaopen_string, void, (lua_State* L)) {
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
