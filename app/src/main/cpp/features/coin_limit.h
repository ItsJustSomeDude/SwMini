#ifndef NEEDLE_COIN_LIMIT_H
#define NEEDLE_COIN_LIMIT_H

#include "lua.h"

int miniCL_set_from_lua(lua_State *L);

void miniCL_set_default(unsigned short limit);
void miniCL_set(unsigned short limit);
void miniCL_set_too_rich(unsigned short amount);
void miniCL_reset(void);

#endif //NEEDLE_COIN_LIMIT_H
