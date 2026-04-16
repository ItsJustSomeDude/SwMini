#include "lua.h"
#include "lauxlib.h"
#include "core/log.h"
#include "core/hooks.h"
#include "caver/program_state.h"
#include "mini_character.h"

#define LOG_TAG "MiniLuaCharacter"

STATIC_DL_FUNCTION_SYMBOL(
	ExperienceBar_UpdateExperience,
	"_ZN5Caver13ExperienceBar16UpdateExperienceEv",
	void, (void* this)
)

STATIC_DL_FUNCTION_SYMBOL(
	CharacterState_ExperiencePointsRequiredForLevel,
	"_ZN5Caver14CharacterState32ExperiencePointsRequiredForLevelEi",
	int, (void* this, int level)
)

/* To find the offsets for Hiro's Object, Level, Exp, and the Level-up Force Float, go to
 * Caver::GameSceneController::HandleGameEvent, and look in the middle of "case 7". It's
 * directly before the ExperienceBar::UpdateExperience call. */

static int get_level(lua_State *L) {
	void *hero = hero_object_from_L(L);

	int level = *$(int, hero, 0x64, 0xb8);
	lua_pushinteger(L, level);
	return 1;
}

static int get_exp(lua_State *L) {
	void *hero = hero_object_from_L(L);

	int exp = *$(int, hero, 0x60, 0xb4);
	lua_pushinteger(L, exp);
	return 1;
}

static int set_level(lua_State *L) {
	void *hero = hero_object_from_L(L);

	int new_level = (int) luaL_checkinteger(L, 1);
	*$(int, hero, 0x64, 0xb8) = new_level;

	return 0;
}


static int set_exp(lua_State *L) {
	void *scene_controller = scene_controller_from_L(L);
	void *hero = hero_object_from_L(L);

	int new_exp = (int) luaL_checkinteger(L, 1);
	int old_exp = *$(int, hero, 0x60, 0xb4);

	/* Set Exp in RAM */
	*$(int, hero, 0x60, 0xb4) = new_exp;

	/* If this value is not set, a level up will not be triggered if the next level is reached. */
	*$(float, scene_controller, 0x100, 0x17c) = 0.2f;

	if (old_exp != new_exp) {
		/* Fetch a reference to the Exp bar, and update it. */
		void *in_between = *$(void*, scene_controller, 0x18, 0x30);
		void *exp_bar = *$(void*, in_between, 0x15c, 0x218);
		ExperienceBar_UpdateExperience(exp_bar);
	}

	return 0;
}

static int exp_for_level(lua_State *L) {
	void *scene_controller = scene_controller_from_L(L);
	void *hero = hero_object_from_L(L);

	/* Don't dereference! */
	void *character_state = $(void*, hero, 0x8, 0x10);

	int level = (int) luaL_checkinteger(L, 1);
	int required = CharacterState_ExperiencePointsRequiredForLevel(character_state, level);

	lua_pushinteger(L, required);
	return 1;
}

static int get_level_attributes(lua_State *L) {
	void *scene_controller = scene_controller_from_L(L);
	void *hero = *$(void*, scene_controller, 0x4, 0x8);
	if (hero == NULL)
		return luaL_error(L, "Unable to find Hero object!");

	lua_pushinteger(L, *$(int, hero, 0x0, 0xbc));
	lua_pushinteger(L, *$(int, hero, 0x0, 0xc0));
	lua_pushinteger(L, *$(int, hero, 0x0, 0xc4));

	return 3;
}

static int set_level_attributes(lua_State *L) {
	void *scene_controller = scene_controller_from_L(L);
	void *hero = hero_object_from_L(L);

	int health = luaL_optinteger(L, 1, INT_MIN);
	int attack = luaL_optinteger(L, 2, INT_MIN);
	int magic = luaL_optinteger(L, 3, INT_MIN);

	/* Prevent saves from becoming softlocked due to health being too low... */
	if (health < -1)
		health = -1;

	if (health != INT_MIN)
		*$(int, hero, 0x0, 0xbc) = health;
	if (attack != INT_MIN)
		*$(int, hero, 0x0, 0xc0) = attack;
	if (magic != INT_MIN)
		*$(int, hero, 0x0, 0xc4) = magic;

	return 0;
}

const luaL_Reg exp_functions[] = {
	{"GetLevel",           get_level},
	{"SetLevel",           set_level},
	{"GetExp",             get_exp},
	{"SetExp",             set_exp},
	{"ExpForLevel",        exp_for_level},
	{"GetLevelAttributes", get_level_attributes},
	{"SetLevelAttributes", set_level_attributes},
	{NULL, NULL}
};

void initLL_character_exp() {
	dlsym_ExperienceBar_UpdateExperience();
	dlsym_CharacterState_ExperiencePointsRequiredForLevel();
}
