#include "component.h"

#include "caver.h"

DL_FUNCTION_SYMBOL(
	ComponentWithInterface,
	"_ZNK5Caver11SceneObject22ComponentWithInterfaceEl",
	void*, (void *scene_object, void *interface)
)

void init_caver_components() {
	dlsym_ComponentWithInterface();
}
