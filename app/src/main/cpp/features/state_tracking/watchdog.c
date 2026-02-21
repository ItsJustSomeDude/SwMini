#include "hooks.h"
#include "lua.h"
#include "state_tracking.h"

#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>

#define LOG_TAG "MiniWatchdog"

static int64_t currentTimeMillis() {
	struct timeval time;
	gettimeofday(&time, NULL);
	int64_t s1 = (int64_t) (time.tv_sec) * 1000;
	int64_t s2 = (time.tv_usec / 1000);
	return s1 + s2;
}

void mark_resumed(lua_State *L) {
	long now = currentTimeMillis();
	StateInfo *si = get_state_info(L);
	si->last_yield = now;
}

void mark_suspended(lua_State *L) {
	long now = currentTimeMillis();
	StateInfo *si = get_state_info(L);
	si->last_resume = now;
}

STATIC_DL_HOOK_ADDR(resume, lua_resume, int, (lua_State * L, int narg)) {
	mark_resumed(L);
	return orig_resume(L, narg);
}

STATIC_DL_HOOK_ADDR(yield, lua_yield, int, (lua_State * L, int nresults)) {
	mark_suspended(L);
	return orig_yield(L, nresults);
}

void setup_state_watchdog() {
	//	hook_resume();
	//	hook_yield();
}
