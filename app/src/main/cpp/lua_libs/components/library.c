#include <string.h>
#include "components.h"
#include "lua.h"
#include "lauxlib.h"
#include "caver/scene_object.h"
#include "caver/components.h"
#include "core/cpp_strings.h"

/* void* pointer to the component's interface. */
#define UPV_INTERFACE lua_upvalueindex(1)

/* const char* pointer to name of component. */
#define UPV_COMPONENT_NAME lua_upvalueindex(2)

/* pointer to NULL-terminated C array of field names/functions.
 * Only present on get_/set_all closures. */
#define UPV_FIELDS lua_upvalueindex(3)

static SceneObject *get_scene_object(lua_State *L, int index) {
	SceneObject **obj = luaL_checkudata(L, index, "SceneObject");
	if (obj == NULL || *obj == NULL) {
		luaL_argerror(L, index, "Invalid SceneObject");
		return NULL;
	}
	return *obj;
}

static Component *get_component(
	lua_State *L, SceneObject *obj, void *interface, const char *component_name
) {
	void *component = SceneObject_ComponentWithInterface(obj, interface);
	if (component == NULL) {
		CppString *obj_id = *$(CppString*, obj, 0x2c, 0x50);
		luaL_error(L, "SceneObject '%s' does not have '%sComponent'.", obj_id, component_name);
		return NULL;
	}
	return component;
}

/* `Component.Upv2.SetField(obj, field, new_value)` */
static int closure_set_field(lua_State *L) {
	void *interface = lua_touserdata(L, UPV_INTERFACE);
	const char *comp_name = lua_tostring(L, UPV_COMPONENT_NAME);
	ComponentField *fields = lua_touserdata(L, UPV_FIELDS);

	SceneObject *obj = get_scene_object(L, 1);
	Component *component = get_component(L, obj, interface, comp_name);

	const char *field_name = luaL_checkstring(L, 2);
	luaL_checkany(L, 3);
	lua_settop(L, 3);

	ComponentField *cur = fields;
	for (; cur->name; cur++) {
		if (strcmp(cur->name, field_name) != 0) continue;

		cur->action(L, component); /* Desired value is on top already. */
		return 0;
	}

	return luaL_error(L, "Unknown Field in %sComponent: '%s'", comp_name, field_name);
}

/* `Component.Upv2.GetField(obj, field)` */
static int closure_get_field(lua_State *L) {
	void *interface = lua_touserdata(L, UPV_INTERFACE);
	const char *comp_name = lua_tostring(L, UPV_COMPONENT_NAME);
	ComponentField *fields = lua_touserdata(L, UPV_FIELDS);

	SceneObject *obj = get_scene_object(L, 1);
	Component *component = get_component(L, obj, interface, comp_name);

	const char *field_name = luaL_checkstring(L, 2);
	lua_settop(L, 2);

	ComponentField *cur = fields;
	for (; cur->name; cur++) {
		if (strcmp(cur->name, field_name) != 0) continue;

		lua_pushnil(L);
		cur->action(L, component); /* Desired value is on top already. */
		return 1;
	}

	return luaL_error(L, "Unknown Field in %sComponent: '%s'", comp_name, field_name);
}

/* `Component.Upv2.GetFields(obj)` */
static int closure_get_all(lua_State *L) {
	void *interface = lua_touserdata(L, UPV_INTERFACE);
	const char *comp_name = lua_tostring(L, UPV_COMPONENT_NAME);
	ComponentField *fields = lua_touserdata(L, UPV_FIELDS);

	SceneObject *obj = get_scene_object(L, 1);
	Component *component = get_component(L, obj, interface, comp_name);

	lua_settop(L, 1); /* Remove any extra things from the stack. */
	lua_newtable(L); /* Create output table. position 2 */

	ComponentField *cur = fields;
	for (; cur->name; cur++) {
		lua_pushnil(L);
		cur->action(L, component);
		/* "action" leaves value on top of the stack in get mode. */

		lua_setfield(L, 2, cur->name);
		lua_settop(L, 2); /* Reset stack height to ensure the function didn't mess it up. */
	}
	/* Done, return table. */
	return 1;
}

/* `Component.Upv2.SetFields(obj, { ...fields })` */
static int closure_set_all(lua_State *L) {
	void *interface = lua_touserdata(L, UPV_INTERFACE);
	const char *comp_name = lua_tostring(L, UPV_COMPONENT_NAME);
	ComponentField *fields = lua_touserdata(L, UPV_FIELDS);

	SceneObject *obj = get_scene_object(L, 1);
	Component *component = get_component(L, obj, interface, comp_name);

	luaL_checktype(L, 2, LUA_TTABLE);

	ComponentField *cur = fields;
	for (; cur->name; cur++) {
		lua_getfield(L, 2, cur->name);
		if (lua_isnil(L, -1)) continue;

		cur->action(L, component);

		lua_settop(L, 2);
	}
	return 0;
}

/* `Component.Upv2.IsPresent(obj)` */
static int closure_has_component(lua_State *L) {
	void *interface = lua_touserdata(L, UPV_INTERFACE);

	SceneObject *obj = get_scene_object(L, 1);
	void *component = SceneObject_ComponentWithInterface(obj, interface);

	lua_pushboolean(L, component != NULL);
	return 1;
}

/* Creates a specific Component's table, adding it to the table on top of the stack. */
void miniLL_components_add(
	lua_State *L,
	const char *component_name,
	void *interface,
	const ComponentField fields[]
) {
	lua_createtable(L, 0, 5); /* 0 number keys, 5 string keys */
	lua_pushvalue(L, -1);
	lua_setfield(L, -3, component_name);

	static const luaL_Reg closures[] = {
		{"GetValue",  closure_get_field},
		{"SetValue",  closure_set_field},
		{"GetValues", closure_get_all},
		{"SetValues", closure_set_all},
		{"IsPresent", closure_has_component},
		{NULL, NULL}
	};

	/* Push all functions, with upvalues. */
	const luaL_Reg *closure = closures;
	for (; closure->name; closure++) {
		lua_pushlightuserdata(L, interface);
		lua_pushstring(L, component_name);
		lua_pushlightuserdata(L, (void *) fields);
		lua_pushcclosure(L, closure->func, 3);
		lua_setfield(L, -2, closure->name);
	}

	lua_pop(L, 1);
}
