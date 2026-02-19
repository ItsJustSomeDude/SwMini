#include "hooks.h"
#include "log.h"
#include "lua.h"
#include "patches.h"

#include <stdbool.h>
#include <stddef.h>

#define LOG_TAG "MiniPanic"

STATIC_DL_HOOK_SYMBOL(
	programExecute, "_ZN5Caver12ProgramState7ExecuteEi",
	void, (void* this, int stackIndex)
) {
	lua_State *L = *(lua_State **) (this);

	// this->field1_0x8 == NULL
	if (*$(void*, this, 0x4, 0x8) == NULL) {
		// Not a thread, use pcall.
		int result = lua_pcall(L, stackIndex, 0, 0);

		if (result == 0) {
			// Success. Engine just returns in this case.
			return;
		} else if (result == LUA_ERRRUN) {
			const char *err = lua_tostring(L, -1);
			LOGE("Lua Runtime Error: %s", err);
		} else {
			LOGE("pcall returned probably a failure I think... %d", result);
		}

		// No matter the error, set the stack as the engine does.
		// Success already early returned.
		lua_settop(L, -2);
	} else {
		// Engine does this
		*$(long, this, 0x24, 0x48) = 0;

		int result = lua_resume(L, stackIndex);

		if (result == 0) {
			// Success
		} else if (result == LUA_YIELD) {
			// LOGD("Thread yielded.");
		} else /* if (result == LUA_ERRRUN) */ {
			const char *err = lua_tostring(L, -1);
			LOGE("Lua Thread Instant Runtime Error: %s", err);
		}

		if (result != 1) {
			// Same logic as the engine I guess.
			*$(long, this, 0x2f, 0x53) = 1;
		}
	}
}

STATIC_DL_FUNCTION_SYMBOL(
	getSpeedMultiplier, "_ZNK5Caver11SceneObject21updateSpeedMultiplierEv",
	float, (void* sceneObject)
)

STATIC_DL_HOOK_SYMBOL(
	programUpdate, "_ZN5Caver12ProgramState6UpdateEf",
	void, (void* this, float deltaTime)
) {
	bool condition1 = *$(bool, this, 0x2d, 0x51);
	bool paused = *$(bool, this, 0x2e, 0x52);

	// If a is true or b is true, execute the whole function.
	// To speed up the whole logic, skip even the original call, since it would just check the same conditions and return.
	if (!condition1 && !paused) return;

	// this->sceneObject
	void *thisObject = *$(void*, this, 0x10, 0x20);
	// The engine appears to always have this set to 1.0, but check anyway.
	// this->speedScaling
	float speedScaling = *$(float, this, 0x30, 0x54);
	// this->isSuspended
	int isSuspended = *$(int, this, 0x24, 0x48);

	float finalMultiplier;
	float scaledDelta = deltaTime;

	if (thisObject != NULL) {
		finalMultiplier = getSpeedMultiplier(thisObject);
		scaledDelta = finalMultiplier * deltaTime;
	}
	finalMultiplier = scaledDelta * speedScaling;

	if (isSuspended == 1) {
		// Count down the timer.
		// this->sleepTime
		float currentTimer = *$(float, this, 0x28, 0x4c);
		float loweredTimer = currentTimer - finalMultiplier;
		// this->sleepTime = loweredTimer;
		*$(float, this, 0x28, 0x4c) = loweredTimer;

		if (loweredTimer < 0.0) {
			// this->isSuspended = 0;
			*$(int, this, 0x24, 0x48) = 0;

			lua_State *L = *(lua_State **) (this);
			int result = lua_resume(L, 0);

			if (result == 0) {
				// Success
			} else if (result == LUA_YIELD) {
				// LOGD("Thread yielded during Update");
			} else /* if (result == LUA_ERRRUN) */ {
				const char *err = lua_tostring(L, -1);
				LOGE("Lua Thread Update Runtime Error: %s", err);
			}

			if (result != 1) {
				// Same logic as the engine I guess.
				// this->completed = true;
				*$(long, this, 0x2f, 0x53) = 1;
			}
		}
	}

	// At this point, we've done all the logic before...
	// plVar7 = this->field2_0x10;
	// while...
	// I've set this->isSuspended to 0, so a repeat of my logic will not be run by the vanilla function.
	orig_programUpdate(this, deltaTime);
}

STATIC_DL_HOOK_SYMBOL(
	programResume, "_ZN5Caver12ProgramState6ResumeEi",
	void, (void* this, int stackIndex)
) {
//    LOGD("ProgramState Resume");
	lua_State *L = *(lua_State **) (this);

	// this->isSuspended = 0;
	*$(long, this, 0x24, 0x48) = 0;

	int result = lua_resume(L, stackIndex);
	if (result == 0) {
		// Success
	} else if (result == LUA_YIELD) {
		// LOGD("Lua Thread yielded during Resume.");
	} else /* if (result == LUA_ERRRUN) */ {
		const char *err = lua_tostring(L, -1);
		LOGE("Lua Thread Runtime Error during Resume: %s", err);
	}

	if (result != 1) {
		*$(long, this, 0x2f, 0x53) = 1;
	}
}

void init_patch_panic() {
	LOGD("Applying Lua Panic patch");

	dlsym_getSpeedMultiplier();

	hook_programExecute();
	hook_programResume();

#ifdef __aarch64__
	hook_programUpdate();

	// This redirect skips over the logic that the hook does.
	// It prevents the timer from ticking down twice.
	// TODO: Make this use an offset from the start of the symbol, instead of full library offsets.
	branch_within_engine(0x4c164c, 0x4c1684, false);
#endif
	// On 32bit, ProgramState::Update calls ProgramState::Resume, while on 64 bit the resume was inlined.
	// That's why we only place the hook on 64bit builds.

//    hook_programPanic();
//    hook_atPanic();
//    hook_atP2();
}
