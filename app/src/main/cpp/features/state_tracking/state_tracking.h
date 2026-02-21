#ifndef NEEDLE_STATE_TRACKING_H
#define NEEDLE_STATE_TRACKING_H

#include "lua.h"

#include <stdbool.h>

typedef struct StateInfo {
	lua_State *L;
	long last_resume;
	long last_yield;
	bool is_thread;
} StateInfo;

StateInfo *add_state(lua_State *L);
void remove_state(lua_State *L);
StateInfo *get_state_info(lua_State *L);

void setup_state_watchdog();
void setup_state_tracking_hooks();
void init_feature_state_tracking();

#endif //NEEDLE_STATE_TRACKING_H
