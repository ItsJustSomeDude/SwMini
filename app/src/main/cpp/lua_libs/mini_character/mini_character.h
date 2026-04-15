#ifndef NEEDLE_MINI_CHARACTER_H
#define NEEDLE_MINI_CHARACTER_H

#include "lauxlib.h"

extern const luaL_Reg exp_functions[];
extern const luaL_Reg attrib_functions[];

/* I try to keep `init` functions out of most headers, but this header is internal to this Lua Lib,
 * so it's ok.
 * (This library's "public" header is the "lua_libs/libs.h" one that all libraries share.) */
void initLL_character_exp(void);
//void initLL_character_attrib(void);

#endif //NEEDLE_MINI_CHARACTER_H
