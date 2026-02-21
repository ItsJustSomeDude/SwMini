#include "hooks.h"

#include "log.h"
#include "lua.h"
#include "state_tracking.h"

#include <stdbool.h>

#define LOG_TAG "MiniStateHooks"

STATIC_DL_HOOK_ADDR(newstate, lua_newstate, lua_State *, (lua_Alloc allocator, void *ud)) {
	lua_State *state = orig_newstate(allocator, ud);
	LOGD("Created new Lua State at %p", state);

	add_state(state);

	return state;
}

STATIC_DL_HOOK_ADDR(newthread, lua_newthread, lua_State *, (lua_State * L)) {
	lua_State *thread = orig_newthread(L);
	LOGD("Created new Lua Thread %p in State %p", thread, L);

	//	Can't do this, because we need to remove the threads on GC...
	StateInfo *si = add_state(thread);
	si->is_thread = true;

	return thread;
}

STATIC_DL_HOOK_ADDR(close, lua_close, void, (lua_State * L)) {
	orig_close(L);
	LOGD("Closed Lua State %p", L);

	remove_state(L);
}

STATIC_DL_HOOK_OFFSET(freethread, archSplit(0, 0x4f30a8), void, (lua_State * L, lua_State * L1)) {
	orig_freethread(L, L1);
	LOGD("A thread was obliterated! %p out of %p", L1, L);

	remove_state(L1);
}

void setup_state_tracking_hooks() {
	hook_newstate();
	hook_newthread();
	hook_close();
	hook_freethread();
}
