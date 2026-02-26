/*
** $Id: loadlib.c,v 1.52.1.4 2009/09/09 13:17:16 roberto Exp $
** Dynamic library loader for Lua
** See Copyright Notice in lua.h
**
** This module contains an implementation of loadlib for Unix systems
** that have dlfcn, an implementation for Darwin (Mac OS X), an
** implementation for Windows, and a stub for other systems.
*/

#include <stdio.h>
#include <string.h>

#define loadlib_c
#define LUA_LIB

#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
/* Mini */
#include "features/mini_files/patched_functions.h"
#include "log.h"
#include "luaconf.h"

#define LOG_TAG "LuaM_loadlib"

/* This is what Mini will always use as the Lua search path.
 * Relative paths are not included, because CWD does not exist. */
#define MINI_ROCKS_PATHS                                                                           \
    "/ExternalFiles/rocks/?.lua;"                                                                  \
    "/ExternalFiles/rocks/?/init.lua;"                                                             \
    "/Files/rocks/?.lua;"                                                                          \
    "/Files/rocks/?/init.lua;"                                                                     \
    "/Assets/rocks/?.lua;"                                                                         \
    "/Assets/rocks/?/init.lua"

/* prefix for open functions in C libraries */
#define LUA_POF        "luaopen_"

/* separator for open functions in C libraries */
#define LUA_OFSEP    "_"


#define LIBPREFIX    "LOADLIB: "

#define POF        LUA_POF
#define LIB_FAIL    "open"


/* error codes for ll_loadfunc */
#define ERRLIB        1
#define ERRFUNC        2

#define setprogdir(L)        ((void)0)


static void ll_unloadlib(void *lib);
static void *ll_load(lua_State *L, const char *path);
static lua_CFunction ll_sym(lua_State *L, void *lib, const char *sym);


#if defined(LUA_DL_DLOPEN)
/*
** {========================================================================
** This is an implementation of loadlib based on the dlfcn interface.
** The dlfcn interface is available in Linux, SunOS, Solaris, IRIX, FreeBSD,
** NetBSD, AIX 4.2, HPUX 11, and  probably most other Unix flavors, at least
** as an emulation layer on top of native functions.
** =========================================================================
*/

#include <dlfcn.h>

static void ll_unloadlib(void *lib) {
	// Mini: dlclose is dangerous on Android, so don't call it.
	LOGD("Rock handle %p is now inactive.", lib);
	// dlclose(lib);
}

/** Calls dlopen, but pushes a string if it fails to load. */
static void *ll_load(lua_State *L, const char *path) {
	void *lib = dlopen(path, RTLD_NOW);
	if (lib == NULL) lua_pushstring(L, dlerror());
	return lib;
}

/** Calls dlsym using a handle and complete symbol. Pushes an error string if there's an error. */
static lua_CFunction ll_sym(lua_State *L, void *lib, const char *sym) {
	lua_CFunction f = (lua_CFunction) dlsym(lib, sym);
	if (f == NULL) lua_pushstring(L, dlerror());
	return f;
}

/* }====================================================== */



#elif defined(LUA_DL_DLL)
/*
** {======================================================================
** This is an implementation of loadlib for Windows using native functions.
** =======================================================================
*/

#include <windows.h>


#undef setprogdir

static void setprogdir (lua_State *L) {
  char buff[MAX_PATH + 1];
  char *lb;
  DWORD nsize = sizeof(buff)/sizeof(char);
  DWORD n = GetModuleFileNameA(NULL, buff, nsize);
  if (n == 0 || n == nsize || (lb = strrchr(buff, '\\')) == NULL)
	luaL_error(L, "unable to get ModuleFileName");
  else {
	*lb = '\0';
	luaL_gsub(L, lua_tostring(L, -1), LUA_EXECDIR, buff);
	lua_remove(L, -2);  /* remove original string */
  }
}


static void pusherror (lua_State *L) {
  int error = GetLastError();
  char buffer[128];
  if (FormatMessageA(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
	  NULL, error, 0, buffer, sizeof(buffer), NULL))
	lua_pushstring(L, buffer);
  else
	lua_pushfstring(L, "system error %d\n", error);
}

static void ll_unloadlib (void *lib) {
  FreeLibrary((HINSTANCE)lib);
}


static void *ll_load (lua_State *L, const char *path) {
  HINSTANCE lib = LoadLibraryA(path);
  if (lib == NULL) pusherror(L);
  return lib;
}


static lua_CFunction ll_sym (lua_State *L, void *lib, const char *sym) {
  lua_CFunction f = (lua_CFunction)GetProcAddress((HINSTANCE)lib, sym);
  if (f == NULL) pusherror(L);
  return f;
}

/* }====================================================== */



#elif defined(LUA_DL_DYLD)
/*
** {======================================================================
** Native Mac OS X / Darwin Implementation
** =======================================================================
*/

#include <mach-o/dyld.h>


/* Mac appends a `_' before C function names */
#undef POF
#define POF	"_" LUA_POF


static void pusherror (lua_State *L) {
  const char *err_str;
  const char *err_file;
  NSLinkEditErrors err;
  int err_num;
  NSLinkEditError(&err, &err_num, &err_file, &err_str);
  lua_pushstring(L, err_str);
}


static const char *errorfromcode (NSObjectFileImageReturnCode ret) {
  switch (ret) {
	case NSObjectFileImageInappropriateFile:
	  return "file is not a bundle";
	case NSObjectFileImageArch:
	  return "library is for wrong CPU type";
	case NSObjectFileImageFormat:
	  return "bad format";
	case NSObjectFileImageAccess:
	  return "cannot access file";
	case NSObjectFileImageFailure:
	default:
	  return "unable to load library";
  }
}


static void ll_unloadlib (void *lib) {
  NSUnLinkModule((NSModule)lib, NSUNLINKMODULE_OPTION_RESET_LAZY_REFERENCES);
}


static void *ll_load (lua_State *L, const char *path) {
  NSObjectFileImage img;
  NSObjectFileImageReturnCode ret;
  /* this would be a rare case, but prevents crashing if it happens */
  if(!_dyld_present()) {
	lua_pushliteral(L, "dyld not present");
	return NULL;
  }
  ret = NSCreateObjectFileImageFromFile(path, &img);
  if (ret == NSObjectFileImageSuccess) {
	NSModule mod = NSLinkModule(img, path, NSLINKMODULE_OPTION_PRIVATE |
					   NSLINKMODULE_OPTION_RETURN_ON_ERROR);
	NSDestroyObjectFileImage(img);
	if (mod == NULL) pusherror(L);
	return mod;
  }
  lua_pushstring(L, errorfromcode(ret));
  return NULL;
}


static lua_CFunction ll_sym (lua_State *L, void *lib, const char *sym) {
  NSSymbol nss = NSLookupSymbolInModule((NSModule)lib, sym);
  if (nss == NULL) {
	lua_pushfstring(L, "symbol " LUA_QS " not found", sym);
	return NULL;
  }
  return (lua_CFunction)NSAddressOfSymbol(nss);
}

/* }====================================================== */



#else
//#region Fallback
/*
** {======================================================
** Fallback for other systems
** =======================================================
*/

#undef LIB_FAIL
#define LIB_FAIL	"absent"


#define DLMSG	"dynamic libraries not enabled; check your Lua installation"


static void ll_unloadlib (void *lib) {
  (void)lib;  /* to avoid warnings */
}


static void *ll_load (lua_State *L, const char *path) {
  (void)path;  /* to avoid warnings */
  lua_pushliteral(L, DLMSG);
  return NULL;
}


static lua_CFunction ll_sym (lua_State *L, void *lib, const char *sym) {
  (void)lib; (void)sym;  /* to avoid warnings */
  lua_pushliteral(L, DLMSG);
  return NULL;
}

/* }====================================================== */
//#endregion
#endif

/**
 * Given a C library path, returns (C) a Userdata that will hold the dlopen handle.
 * If there's already a registry entry, it returns it.
 * Otherwise, it creates a userdata of size `sizeof (void*)`, sets the metatable to the _LOADLIB mt,
 * stores it to the registry, and returns it.
 *
 * Since we're on Android, "absolute path" is just the library name: rock_<library>.so
 *
 * **Unmodified from vanilla.**
 */
static void **ll_register(lua_State *L, const char *path) {
	void **plib;
	lua_pushfstring(L, "%s%s", LIBPREFIX, path);
	lua_gettable(L, LUA_REGISTRYINDEX);  /* check library in registry? */
	if (!lua_isnil(L, -1))  /* is there an entry? */
		plib = (void **) lua_touserdata(L, -1);
	else {  /* no entry yet; create one */
		lua_pop(L, 1);
		plib = (void **) lua_newuserdata(L, sizeof(const void *));
		*plib = NULL;
		luaL_getmetatable(L, "_LOADLIB");
		lua_setmetatable(L, -2);
		lua_pushfstring(L, "%s%s", LIBPREFIX, path);
		lua_pushvalue(L, -2);
		lua_settable(L, LUA_REGISTRYINDEX);
	}
	return plib;
}


/*
** __gc tag method: calls library's `ll_unloadlib' function with the lib
** handle
*/
static int gctm(lua_State *L) {
	void **lib = (void **) luaL_checkudata(L, 1, "_LOADLIB");
	if (*lib) ll_unloadlib(*lib);
	*lib = NULL;  /* mark library as closed */
	return 0;
}

/** path is absolute, sym is exact symbol name.
 * Since we're on Android, "absolute" .so paths are completely bare, just "lib.so". */
static int ll_loadfunc(lua_State *L, const char *path, const char *sym) {
	void **reg = ll_register(L, path);
	if (*reg == NULL) *reg = ll_load(L, path);
	if (*reg == NULL)
		return ERRLIB;  /* unable to load library */
	else {
		lua_CFunction f = ll_sym(L, *reg, sym);
		if (f == NULL)
			return ERRFUNC;  /* unable to find function */
		lua_pushcfunction(L, f);
		return 0;  /* return function */
	}
}

/**
 * This is the builtin Lua "Raw .so Loader."
 *
 * `_G.package.loadlib(libname, funcname)`
 *
 * Libname must be absolute path, funcname must be exact name of symbol inside library.
 */
static int ll_loadlib(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	const char *init = luaL_checkstring(L, 2);
	int stat = ll_loadfunc(L, path, init);
	if (stat == 0)  /* no errors? */
		return 1;  /* return the loaded function */
	else {  /* error; error message is on stack top */
		lua_pushnil(L);
		lua_insert(L, -2);
		lua_pushstring(L, (stat == ERRLIB) ? LIB_FAIL : "init");
		return 3;  /* return nil, error message, and where */
	}
}


/*
** {======================================================
** 'require' function
** =======================================================
*/

/** This takes an *absolute* file path, and returns 1 if it's readable. */
static int readable(const char *filename) {
	FILE *f = fopen(filename, "r");  /* try to open file */
	if (f == NULL) return 0;  /* open failed */
	fclose(f);
	return 1;
}

/** Given a C String with separators, it pushes the first one, and returns a pointer to the path
 * string shifted to remove the one that was pushed.
 * Input: "test;test2;test3"
 * Pushes: "test", returns "test2;test3" */
static const char *pushnexttemplate(lua_State *L, const char *path) {
	const char *l;
	while (*path == *LUA_PATHSEP) path++;  /* skip separators */
	if (*path == '\0') return NULL;  /* no more templates */
	l = strchr(path, *LUA_PATHSEP);  /* find next separator */
	if (l == NULL) l = path + strlen(path);
	lua_pushlstring(L, path, l - path);  /* template */
	return l;
}

/** Takes a name, and a pname?
 *
 * Changes . in the name to /
 * Looks in the C Environment ?, and finds a string pointed to by pname ?...
 * 		path = Env[pname]; pname is only ever "path" or "cpath".
 * If path is null, throw an error.
 *
 * It now loops through the Path (split by ;), and for each one it substitutes "?" for "name", and
 * checks if it's a readable file.
 *
 * Triggers LongJmp if pname is invalid, returns the found file if found and readable.
 * If file is not found in any of the Path/CPaths, it returns NULL, with a string on top of stack with ALL the error messages encountered.
 */
static const char *findfile(lua_State *L, const char *name, const char *pname) {
	const char *path;
	name = luaL_gsub(L, name, ".", LUA_DIRSEP);

	/* Mini: Use static MINI_ROCKS_PATHS as `env.path` does not exist in Mini. */
	/* lua_getfield(L, LUA_ENVIRONINDEX, pname); */
	lua_pushliteral(L, MINI_ROCKS_PATHS);

	path = lua_tostring(L, -1);
	if (path == NULL)
		/* luaL_error(L, LUA_QL("package.%s") " must be a string", pname); */
		luaL_error(L, "Mini: Internal error during package loading!");

	lua_pushliteral(L, ""); /* error accumulator */
	while ((path = pushnexttemplate(L, path)) != NULL) {
		const char *filename;
		filename = luaL_gsub(L, lua_tostring(L, -1), LUA_PATH_MARK, name);
		lua_remove(L, -2);      /* remove path template */
		if (readable(filename)) /* does file exist and is readable? */
			return filename;    /* return that file name */
		lua_pushfstring(L, "\n\tno file " LUA_QS, filename);
		lua_remove(L, -2); /* remove file name */
		lua_concat(L, 2);  /* add entry to possible error message */
	}
	return NULL; /* not found */
}

/** Throw an error about loading. Input Stack: 1 = Module Name, -1 (top) = error message.
 * File name is probably absolute? */
static void loaderror(lua_State *L, const char *filename) {
	luaL_error(L, "error loading module " LUA_QS " from file " LUA_QS ":\n\t%s",
	           lua_tostring(L, 1), filename, lua_tostring(L, -1));
}

/**
 * Tries to find a Lua file (position 1 on stack) to load using "path".
 * In case of unknown file, returns accumulated errors.
 * In case of invalid Lua in file, LongJmp
 * If all is well, return 1.
 */
static int loader_Lua(lua_State *L) {
	const char *filename;
	const char *name = luaL_checkstring(L, 1);
	filename = findfile(L, name, "path");
	if (filename == NULL) return 1;  /* library not found in this path */
	if (luaL_loadfile(L, filename) != 0)
		loaderror(L, filename);
	return 1;  /* library loaded successfully */
}

/** If the modname has a "-" in it, remove the text before the first one.
 * Pushes and returns the modname after the first "-", with . replaced with /, with "luaopen_" prepended. */
static const char *mkfuncname(lua_State *L, const char *modname) {
	const char *funcname;
	const char *mark = strchr(modname, *LUA_IGMARK);
	if (mark) modname = mark + 1;
	funcname = luaL_gsub(L, modname, ".", LUA_OFSEP);
	funcname = lua_pushfstring(L, POF"%s", funcname);
	lua_remove(L, -2);  /* remove 'gsub' result */
	return funcname;
}

/** Mini: Disable this function, use the custom C loader instead.
 * Try to load a C library (position 1 in stack). */
//static int loader_C(lua_State *L) {
//	const char *funcname;
//	const char *name = luaL_checkstring(L, 1);
//	const char *filename = findfile(L, name, "cpath");
//	if (filename == NULL) return 1;  /* library not found in this path */
//	funcname = mkfuncname(L, name);
//	if (ll_loadfunc(L, filename, funcname) != 0)
//		loaderror(L, filename);
//	return 1;  /* library loaded successfully */
//}

/** Mini: Disable this function, use the custom CRoot loader instead. */
//static int loader_Croot(lua_State *L) {
//	const char *funcname;
//	const char *filename;
//	const char *name = luaL_checkstring(L, 1);
//	const char *p = strchr(name, '.');
//	int stat;
//	if (p == NULL) return 0;  /* is root */
//	lua_pushlstring(L, name, p - name);
//	filename = findfile(L, lua_tostring(L, -1), "cpath");
//	if (filename == NULL) return 1;  /* root not found */
//	funcname = mkfuncname(L, name);
//	if ((stat = ll_loadfunc(L, filename, funcname)) != 0) {
//		if (stat != ERRFUNC) loaderror(L, filename);  /* real error */
//		lua_pushfstring(L, "\n\tno module " LUA_QS " in file " LUA_QS,
//		                name, filename);
//		return 1;  /* function not found */
//	}
//	return 1;
//}

#define ROCK_PREFIX "rock_"
#define ROCK_SUFFIX ".so"

/**
 * Attempts to load a native rock by name, returns handle on success.
 * Returns NULL and pushes error string on failure.
 *
 * Also pushes the name of the .so file that was loaded to the buffer if not NULL.
 * Caller is responsible for making sure it's large enough.
 */
static void *load_native_rock(
	lua_State *L, const char *name, char *lib_name_buffer, size_t buf_size) {
	snprintf(lib_name_buffer, buf_size, ROCK_PREFIX "%s" ROCK_SUFFIX, name);
	LOGD("C Loader: Attempting to load Rock %s", lib_name_buffer);

	void *handle = dlopen(lib_name_buffer, RTLD_GLOBAL | RTLD_NOW);
	if (handle == NULL) {
		/* dlopen reported error, either does not exist or is invalid. */
		const char *err = dlerror();
		if (err == NULL) {
			lua_pushfstring(L, "\n\tMini: Loading " LUA_QS " failed silently", lib_name_buffer);
		} else {
			lua_pushfstring(L, "\n\tMini: Loading " LUA_QS " reported: %s", lib_name_buffer, err);
		}
		return NULL;
	}
	return handle;
}

/** Special Loader for Mini. rock_<name>.so will be loaded from `libs` using dlopen, not a path.
 * The input might have . in between stuff, which is actually ok, because that's how it's laid out
 * in the APK's Libs folder. */
static int loader_mini_C(lua_State *L) {
	const char *funcname;
	const char *name = luaL_checkstring(L, 1); /* Name of lib to load. */

	char lib_name[PATH_MAX];

	/* Try to load "rock_<name>.so" */
	const void *handle = load_native_rock(L, name, lib_name, sizeof(lib_name));
	if (handle == NULL) /* Error already pushed. */
		return 1;

	funcname = mkfuncname(L, name);
	if (ll_loadfunc(L, lib_name, funcname) != 0)
		loaderror(L, lib_name);
	return 1;
}

/**  */
static int loader_mini_Croot(lua_State *L) {
	const char *funcname;
	/* Name is like root.thing.thing */
	const char *name = luaL_checkstring(L, 1);

	const char *p = strchr(name, '.');
	int stat;
	if (p == NULL) return 0; /* is root */

	/* Push shortened (root) path to the stack? Add to root_name.
	 * root_name is "root" if name is "root.thing.thing" */
	lua_pushlstring(L, name, p - name);
	const char *root_name = lua_tostring(L, -1);

	char lib_name[PATH_MAX];

	/* Try to load "rock_<root>.so" */
	const void *handle = load_native_rock(L, root_name, lib_name, sizeof(lib_name));
	if (handle == NULL) /* Error already pushed. */
		return 1;

	funcname = mkfuncname(L, name);
	if ((stat = ll_loadfunc(L, lib_name, funcname)) != 0) {
		if (stat != ERRFUNC)
			loaderror(L, lib_name); /* real error */
		lua_pushfstring(L, "\n\tMini: no module " LUA_QS " in rock " LUA_QS, name, lib_name);
		return 1; /* function not found */
	}
	return 1; /* Success! */
}

/** Unmodified from Vanilla. */
static int loader_preload(lua_State *L) {
	const char *name = luaL_checkstring(L, 1);
	lua_getfield(L, LUA_ENVIRONINDEX, "preload");
	if (!lua_istable(L, -1))
		luaL_error(L, LUA_QL("package.preload") " must be a table");
	lua_getfield(L, -1, name);
	if (lua_isnil(L, -1))  /* not found? */
		lua_pushfstring(L, "\n\tno field package.preload['%s']", name);
	return 1;
}

static const int sentinel_ = 0;
#define sentinel    ((void *)&sentinel_)

/**
 * The modified Loaders list, needed for `require`. Changes Mini makes:
 *
 * - Preload is unchanged.
 * -
 */
static const lua_CFunction loaders[] = {
	loader_preload,
	loader_Lua,
	loader_mini_C,
	loader_mini_Croot,
	/* Last loader == NULL, meaning "Failed" */
	NULL
};

/**
 * This is the vanilla Lua `require` function. It uses the `package.loaders` table (although I'd prefer if it didn't...)
 * _G.require("package name")
 *
 * Modified to use Loaders defined in C instead of being available to Lua.
 */
static int ll_require(lua_State *L) {
	const char *name = luaL_checkstring(L, 1);
	int i;

	/* Start by checking in _LOADED (package.loaded). If it's there, return it. */

	lua_settop(L, 1);  /* _LOADED table will be at index 2 */
	lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
	lua_getfield(L, 2, name);
	if (lua_toboolean(L, -1)) {  /* is it there? */
		if (lua_touserdata(L, -1) == sentinel)  /* check loops */
			luaL_error(L, "loop or previous error loading module " LUA_QS, name);
		return 1;  /* package is already loaded */
	}
	/* else must load it; iterate over available loaders */

	lua_pushliteral(L, ""); /* error message accumulator */

	/* Mini: Use static loaders instead of `package.loaders` */
	for (i = 0; i < sizeof(loaders) / sizeof(loaders[0]); i++) {
		if (loaders[i] == NULL) /* Last one indicates failure. */
			/* Accumulator is on top of the stack because we have not pushed the C Function yet. */
			luaL_error(L, "module " LUA_QS " not found:%s", name, lua_tostring(L, -1));

		lua_pushcfunction(L, loaders[i]);
		lua_pushstring(L, name);
		lua_call(L, 1, 1); /* call it */

		if (lua_isfunction(L, -1))    /* did it find module? */
			break;                    /* module loaded successfully */
		else if (lua_isstring(L, -1)) /* loader returned error message? */
			lua_concat(L, 2);         /* accumulate it */
		else
			lua_pop(L, 1);
	}
	lua_pushlightuserdata(L, sentinel);
	lua_setfield(L, 2, name);                /* _LOADED[name] = sentinel */
	lua_pushstring(L, name);                 /* pass name as argument to module */
	lua_call(L, 1, 1);                       /* run loaded module */
	if (!lua_isnil(L, -1))                   /* non-nil return? */
		lua_setfield(L, 2, name);            /* _LOADED[name] = returned value */
	lua_getfield(L, 2, name);                /* Get just-set value, or sentinel. */
	if (lua_touserdata(L, -1) == sentinel) { /* module did not set a value? */
		lua_pushboolean(L, 1);               /* use true as result */
		lua_pushvalue(L, -1);                /* extra copy to be returned */
		lua_setfield(L, 2, name);            /* _LOADED[name] = true */
	}
	return 1;
}

/* }====================================================== */



/*
 * Unmodified from Vanilla.
 *
** {======================================================
** 'module' function
** =======================================================
*/
//#region

static void setfenv(lua_State *L) {
	lua_Debug ar;
	if (lua_getstack(L, 1, &ar) == 0 ||
	    lua_getinfo(L, "f", &ar) == 0 ||  /* get calling function */
	    lua_iscfunction(L, -1))
		luaL_error(L, LUA_QL("module") " not called from a Lua function");
	lua_pushvalue(L, -2);
	lua_setfenv(L, -2);
	lua_pop(L, 1);
}


static void dooptions(lua_State *L, int n) {
	int i;
	for (i = 2; i <= n; i++) {
		lua_pushvalue(L, i);  /* get option (a function) */
		lua_pushvalue(L, -2);  /* module */
		lua_call(L, 1, 0);
	}
}


static void modinit(lua_State *L, const char *modname) {
	const char *dot;
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "_M");  /* module._M = module */
	lua_pushstring(L, modname);
	lua_setfield(L, -2, "_NAME");
	dot = strrchr(modname, '.');  /* look for last dot in module name */
	if (dot == NULL) dot = modname;
	else dot++;
	/* set _PACKAGE as package name (full module name minus last part) */
	lua_pushlstring(L, modname, dot - modname);
	lua_setfield(L, -2, "_PACKAGE");
}


static int ll_module(lua_State *L) {
	const char *modname = luaL_checkstring(L, 1);
	int loaded = lua_gettop(L) + 1;  /* index of _LOADED table */
	lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
	lua_getfield(L, loaded, modname);  /* get _LOADED[modname] */
	if (!lua_istable(L, -1)) {  /* not found? */
		lua_pop(L, 1);  /* remove previous result */
		/* try global variable (and create one if it does not exist) */
		if (luaL_findtable(L, LUA_GLOBALSINDEX, modname, 1) != NULL)
			return luaL_error(L, "name conflict for module " LUA_QS, modname);
		lua_pushvalue(L, -1);
		lua_setfield(L, loaded, modname);  /* _LOADED[modname] = new table */
	}
	/* check whether table already has a _NAME field */
	lua_getfield(L, -1, "_NAME");
	if (!lua_isnil(L, -1))  /* is table an initialized module? */
		lua_pop(L, 1);
	else {  /* no; initialize it */
		lua_pop(L, 1);
		modinit(L, modname);
	}
	lua_pushvalue(L, -1);
	setfenv(L);
	dooptions(L, loaded - 1);
	return 0;
}


static int ll_seeall(lua_State *L) {
	luaL_checktype(L, 1, LUA_TTABLE);
	if (!lua_getmetatable(L, 1)) {
		lua_createtable(L, 0, 1); /* create new metatable */
		lua_pushvalue(L, -1);
		lua_setmetatable(L, 1);
	}
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	lua_setfield(L, -2, "__index");  /* mt.__index = _G */
	return 0;
}

//#endregion
/* }====================================================== */

/* Mini: Disable all this, Paths cannot be loaded from ENV or set from Lua. */
/* auxiliary mark (for internal use) */
//#define AUXMARK        "\1"
//
//static void setpath(lua_State *L, const char *fieldname, const char *envname,
//                    const char *def) {
//	const char *path = getenv(envname);
//	if (path == NULL)  /* no environment variable? */
//		lua_pushstring(L, def);  /* use default */
//	else {
//		/* replace ";;" by ";AUXMARK;" and then AUXMARK by default path */
//		path = luaL_gsub(L, path, LUA_PATHSEP LUA_PATHSEP,
//		                 LUA_PATHSEP AUXMARK LUA_PATHSEP);
//		luaL_gsub(L, path, AUXMARK, def);
//		lua_remove(L, -2);
//	}
//	setprogdir(L);
//	lua_setfield(L, -2, fieldname);
//}

/** These are the functions that are added to the `_G.package` table. */
static const luaL_Reg pk_funcs[] = {
	{"loadlib", ll_loadlib},
	{"seeall",  ll_seeall},
	{NULL, NULL}
};

/** These are the functions that are added to the global scope. */
static const luaL_Reg ll_funcs[] = {
	{"module",  ll_module},
	{"require", ll_require},
	{NULL, NULL}
};

LUALIB_API int luaopen_package(lua_State *L) {
	/* create new type _LOADLIB
	 * This is the metatable for loaded C libraries, which has the __gc metamethod.
	 * We don't actually `dlclose` them, but it still calls the method. */
	luaL_newmetatable(L, "_LOADLIB");
	lua_pushcfunction(L, gctm);
	lua_setfield(L, -2, "__gc");

	/* create `package' table */
	luaL_register(L, LUA_LOADLIBNAME, pk_funcs);
#if defined(LUA_COMPAT_LOADLIB)
	lua_getfield(L, -1, "loadlib");
	lua_setfield(L, LUA_GLOBALSINDEX, "loadlib");
#endif
	lua_pushvalue(L, -1);
	lua_replace(L, LUA_ENVIRONINDEX);

	/* Mini: Remove Loaders table for security. */
	//	/* create `loaders' table */
	//	lua_createtable(L, sizeof(loaders) / sizeof(loaders[0]) - 1, 0);
	//	/* fill it with pre-defined loaders */
	//	for (i = 0; loaders[i] != NULL; i++) {
	//		lua_pushcfunction(L, loaders[i]);
	//		lua_rawseti(L, -2, i + 1);
	//	}
	//	lua_setfield(L, -2, "loaders");  /* put it in field `loaders' */

	/* Mini: CPath is completely unused (just uses `dlopen` linker path).
	 * Path must not be changed by Lua code.
	 * Config information is useless. */
	//	setpath(L, "path", LUA_PATH, LUA_PATH_DEFAULT);  /* set field `path' */
	//	setpath(L, "cpath", LUA_CPATH, LUA_CPATH_DEFAULT); /* set field `cpath' */
	//	/* store config information */
	//	lua_pushliteral(L, LUA_DIRSEP "\n" LUA_PATHSEP "\n" LUA_PATH_MARK "\n"
	//		LUA_EXECDIR "\n" LUA_IGMARK);
	//	lua_setfield(L, -2, "config");

	/* set field `loaded'. `_G.package.loaded == debug.getregistry()._LOADED` */
	luaL_findtable(L, LUA_REGISTRYINDEX, "_LOADED", 2);
	lua_setfield(L, -2, "loaded");

	/* set field `preload' */
	lua_newtable(L);
	lua_setfield(L, -2, "preload");

	lua_pushvalue(L, LUA_GLOBALSINDEX);
	luaL_register(L, NULL, ll_funcs);  /* open lib into global table */
	lua_pop(L, 1);
	return 1;  /* return 'package' table */
}

