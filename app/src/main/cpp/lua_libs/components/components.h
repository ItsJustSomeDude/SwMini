#ifndef NEEDLE_LUA_COMPONENTS_H
#define NEEDLE_LUA_COMPONENTS_H

#include "lua.h"
#include "lauxlib.h"
#include "core/hooks.h"
#include "core/cpp_strings.h"

// Might be cleaner to use this in the future, idk.
//typedef struct ComponentMapping {
//	const char *component_name;
//	void **interface;
//	ComponentField fields[];
//} ComponentMapping;

typedef struct ComponentField {
	const char *name;
	int (*action)(lua_State *L, void *component);
} ComponentField;

void miniLL_components_add(
	lua_State *L, const char *component_name, void *interface, const ComponentField *fields
);

/* Add `extern const ComponentField <name>[]` arrays here. */

extern const ComponentField SwingableWeaponControllerFields[];
extern const ComponentField HealthFields[];
extern const ComponentField EntityFields[];
extern const ComponentField PhysicsFields[];

/* If needed, you can add "init" header entries here.
 * Example:
 * void initLL_components_health(void);
 */

#endif /* NEEDLE_LUA_COMPONENTS_H */
