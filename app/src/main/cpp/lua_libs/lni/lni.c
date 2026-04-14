#include "lni.h"
#include "lua.h"
#include "lauxlib.h"

static int copy(lua_State *L) {
	return lni_exec_name(L, "copyToClipboard");
}

static int open_url(lua_State *L) {
	return lni_exec_name(L, "openUrl");
}

static int set_speed(lua_State *L) {
	return lni_exec_name(L, "setSpeed");
}

static int get_speed(lua_State *L) {
	return lni_exec_name(L, "getSpeed");
}

static int quit(lua_State *L) {
	return lni_exec_name(L, "quit");
}

static const luaL_Reg lni_library[] = {
	{"Execute",         miniLL_lni_execute},
	{"Bind",            miniLL_lni_bind},

	/* The default bound functions: */
	{"copy",            copy},
	{"copyToClipboard", copy},
	{"Copy",            copy},
	{"CopyToClipboard", copy},

	{"openUrl",         open_url},
	{"openURL",         open_url},
	{"OpenUrl",         open_url},
	{"OpenURL",         open_url},

	{"setSpeed",        set_speed},
	{"SetSpeed",        set_speed},

	{"getSpeed",        get_speed},
	{"GetSpeed",        get_speed},

	{"quit",            quit},
	{"Quit",            quit},

	{NULL, NULL}
};

int miniLL_open_lni(lua_State *L) {
	lua_newtable(L);
	luaL_register(L, NULL, lni_library);

	return 1;
}