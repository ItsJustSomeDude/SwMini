#include "armor_models.h"

#include "features/cstrings/cstrings.h"
#include "hooks.h"
#include "libs/khash.h"
#include "log.h"

#include <stdlib.h>
#include <string.h>

#define LOG_TAG "MiniArmorModels"

KHASH_MAP_INIT_STR(str, const char*)

static kh_str_t *models = NULL;
static const char *default_model = NULL;

// These dummy values are here to force Clang to use the sret pointer (x8) return style.
// Don't actually try to read the dummies, they are pure undefined behavior.

typedef struct {
	void *pointer;
	long _dummy2;
	long _dummy3;
} Pointer_ForceParameterReturn;

#ifdef __aarch64__
// On 64bit, return the generated stack structure.
#define _return return ret
#define out ret.pointer

STATIC_DL_HOOK_SYMBOL(
	mnfa, "_ZN5Caver20HeroEquipmentManager17ModelNameForArmorERKN5boost10shared_ptrINS_4ItemEEE",
	Pointer_ForceParameterReturn, (void* this, void** item)
) {
	// Call the original to setup _something_ on the stack.
	Pointer_ForceParameterReturn ret = orig_mnfa(this, item);

#elif defined(__arm__)
// On 32bit, there's no return value.
#define _return (void)0
#define out *output

STATIC_DL_HOOK_SYMBOL(
	mnfa, "_ZN5Caver20HeroEquipmentManager17ModelNameForArmorERKN5boost10shared_ptrINS_4ItemEEE",
	void, (const CppString **output, void *this, void **item)
) {
#endif
	if (*item != NULL) {
		// Get from item ID.
		char *item_id = *$(char*, *item, 0x4, 0x8);
		LOGD("Item ID: %s", item_id);

		// Look up in table.
		khint_t k = kh_get_str(models, item_id);
		if (k != kh_end(models)) {
			// Found!
			CppString *cpp_model = NULL;
			create_basic_string(&cpp_model, kh_val(models, k));
			out = cpp_model;
			_return;
		}
		// Not found, fall through.
	}

	// No or unknown item.
	CppString *cpp_default_model = NULL;
	create_basic_string(&cpp_default_model, default_model);
	out = cpp_default_model;
	_return;
}

void miniAM_add_model(const char *_Nonnull item_name, const char *_Nonnull model_name) {
	if (models == NULL)
		models = kh_init_str();

	// TODO: This will probably leak memory if the same item is added multiple times.

	int absent;
	khint_t k = kh_put_str(models, strdup(item_name), &absent);
	if (absent) {
		LOGD("Added model: '%s' -> '%s'", item_name, model_name);
		kh_value(models, k) = strdup(model_name);
	} else {
		LOGD("Replaced model: '%s' -> '%s'", item_name, model_name);
		const char *old_model = kh_value(models, k);

		if (strcmp(old_model, model_name) == 0)
			return; // Didn't change.

		// Delete old value (keep key).
		free((void *) old_model);

		kh_value(models, k) = strdup(model_name);
	}
}

void miniAM_set_default_model(const char *_Nonnull model_name) {
	LOGD("Setting default player model to '%s'", model_name);

	// Remove old default for safety.
	if (default_model != NULL) {
		if (strcmp(model_name, default_model) == 0) {
			return; // Didn't change.
		}

		free((void *) default_model);
	}

	default_model = strdup(model_name);
}

void initF_armor_models() {
	hook_mnfa();

	miniAM_set_default_model("hiro");
	miniAM_add_model("platearmor", "hiro_plated");
	miniAM_add_model("magicarmor", "hiro_magicplated");
}
