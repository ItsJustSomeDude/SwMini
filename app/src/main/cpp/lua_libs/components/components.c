#include "lua.h"
#include "caver/components.h"
#include "components.h"
#include "init/lua_libs.h"

int miniLL_open_components(lua_State *L) {
	lua_newtable(L);

	miniLL_components_add(L, "Health", HealthComponent_Interface, HealthFields);

	miniLL_components_add(
		L, "SwingableWeaponController",
		SwingableWeaponControllerComponent_Interface, SwingableWeaponControllerFields);

	return 1;
}

void initLL_components(void) {
	/* If you end up needing to dlsym something, call the function here.
	 * Example:
	 * initLL_components_health(); */
}