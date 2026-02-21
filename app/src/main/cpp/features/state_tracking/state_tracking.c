#include "state_tracking.h"

#include "libs/khash.h"
#include "log.h"
#include "lua.h"

#include <stdlib.h>

#define LOG_TAG "MiniStateTracking"

#ifdef __aarch64__
#define cast_t khint64_t
KHASH_MAP_INIT_INT64(state_map, StateInfo *)

#elif defined(__arm__)
#define cast_t khint32_t
KHASH_MAP_INIT_INT(state_map, StateInfo *)

#endif

static kh_state_map_t *states = NULL;

/**
 * Adds a lua_State* to be tracked.
 */
StateInfo *add_state(lua_State *L) {
	StateInfo *si = malloc(sizeof(StateInfo));
	si->L = L;
	si->last_resume = 0;
	si->last_yield = 0;

	// The key in the hash table is the Lua State Pointer.
	int slot_info;
	khint_t insert_index = kh_put_state_map(states, (cast_t) L, &slot_info);
	LOGD("Insertion result: %i in slot %i", slot_info, insert_index);

	if (slot_info == 0) {
		// Existing thing in table!
		LOGE("Tried to add Lua state %p multiple times!", L);
	} else if (slot_info == 1 || slot_info == 2) {
		// New Bucket (1) or Emptied bucket (2)
		kh_value(states, insert_index) = si;
	} else {
		// Error!
		LOGE("Failed to track Lua State: %p", L);
	}
	return si;
}

void remove_state(lua_State *L) {
	khint_t del_index = kh_get_state_map(states, (cast_t) L);
	if (del_index != kh_end(states) && kh_exist(states, del_index)) {
		free(kh_value(states, del_index));
		kh_del_state_map(states, del_index);
		LOGD("Removed Lua State %p from tracking.", L);
	}
}

/** Returns a live reference. */
StateInfo *get_state_info(lua_State *L) {
	khint_t index = kh_get_state_map(states, (cast_t) L);
	if (index != kh_end(states) && kh_exist(states, index)) {
		return kh_value(states, index);
	}
	LOGE("Found nothing... %p", L);
	return NULL;
}

void init_feature_state_tracking() {
	states = kh_init_state_map();

	setup_state_tracking_hooks();
	setup_state_watchdog();
}
