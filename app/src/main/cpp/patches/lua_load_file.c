#include <string.h>
#include <errno.h>
#include "patches.h"
#include "hooks.h"
#include "lauxlib.h"
#include "log.h"
#include "features/mini_files/mini_files.h"
#include "features/mini_files/patched_functions.h"

#define LOG_TAG "MiniLoadFile"

/* This patches luaL_loadfile to take Mini paths.
 * It's basically a copy of the relevant functions from `lauxlib.c`. */

typedef struct LoadF {
	/* Whether a SHEBANG was removed, thus shifting line numbers by 1. */
	int extraline;
	FILE *f;
	char buff[LUAL_BUFFERSIZE];
//	char buff[1];
} LoadF;

static const char *getF(lua_State *L, void *ud, size_t *size) {
	LoadF *lf = (LoadF *) ud;
	(void) L;
	if (lf->extraline) {
		lf->extraline = 0;
		*size = 1;
		return "\n";
	}
	if (miniP_feof(lf->f)) return NULL;
	*size = miniP_fread(lf->buff, 1, sizeof(lf->buff), lf->f);
	return (*size > 0) ? lf->buff : NULL;
}

static int errfile(lua_State *L, const char *what, int fnameindex) {
	const char *serr = strerror(errno);
	const char *filename = lua_tostring(L, fnameindex) + 1;
	lua_pushfstring(L, "cannot %s %s: %s", what, filename, serr);
	lua_remove(L, fnameindex);
	return LUA_ERRFILE;
}

STATIC_DL_HOOK_ADDR(
	load_file, luaL_loadfile,
	int, (lua_State * L, const char *mini_filename)
) {
	/* Begin Mini: */
	MiniPath p;
	parse_mini_path(&p, mini_filename);
	locate_resource(&p);
	if (p.type == ERROR || p.type == UNKNOWN) {
		int top = lua_gettop(L);
		lua_pushfstring(L, "Invalid MiniPath: %s", p.path);
		lua_remove(L, top + 1);
		return LUA_ERRFILE;
	}
	const char *filename = p.path;
	LOGD("Using '%s' from '%s'", filename, mini_filename);

	LoadF lf;
	int status, readstatus;
	int c;
	int fnameindex = lua_gettop(L) + 1;  /* index of filename on the stack */
	lf.extraline = 0;

	/* STDIN loading removed. */

	/* Push Function Name, open the file, return error if failed. */
	lua_pushfstring(L, "@%s", mini_filename);
	lf.f = miniP_fopen(&p, "r");
	if (lf.f == NULL)
		return errfile(L, "open", fnameindex);

	/* Get first char, if it's a shebang read to the end of the line. */
	c = miniP_getc(lf.f);
	if (c == '#') {  /* Unix exec. file? */
		lf.extraline = 1;
		while ((c = miniP_getc(lf.f)) != EOF && c != '\n');  /* skip first line */
		/* No need to read the next char, because the binary check was removed. */
		/* if (c == '\n') c = getc(lf.f); */
	} else {
		/* Seek back 1, so the read starts at the beginning. */
		miniP_fseek(lf.f, -1, SEEK_CUR);
	}

	/* Skip binary check, because it will be fed into `lua_load` all the same,
	 * and there's no reason to reopen because binary files are treated the same. */
//	if (c == LUA_SIGNATURE[0] && filename) {  /* binary file? */
//		lf.f = freopen(filename, "rb", lf.f);  /* reopen in binary mode */
//		if (lf.f == NULL) return errfile(L, "reopen", fnameindex);
//		/* skip eventual `#!...' */
//		while ((c = getc(lf.f)) != EOF && c != LUA_SIGNATURE[0]);
//		lf.extraline = 0;
//	}

	/* No need to push last char back, because the binary check was skipped. */
	/* ungetc(c, lf.f); */

	status = lua_load(L, getF, &lf, lua_tostring(L, -1));
	readstatus = miniP_ferror(lf.f);
	if (filename) miniP_fclose(lf.f);  /* close file (even in case of errors) */
	if (readstatus) {
		lua_settop(L, fnameindex);  /* ignore results from `lua_load' */
		return errfile(L, "read", fnameindex);
	}
	lua_remove(L, fnameindex);
	return status;
}

void initP_lua_load_file() {
	hook_load_file();
}
