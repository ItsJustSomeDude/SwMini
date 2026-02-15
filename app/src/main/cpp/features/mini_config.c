#include <jni.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mini_config.h"
#include "java.h"
#include "libs/toml/toml.h"
#include "config.h"
#include "coin_limit.h"
#include "log.h"
#include "armor_models.h"

#define LOG_TAG "MiniConfig"

void read_mini_config_asset() {
	const char *asset_path = "mini.toml";
	JNIEnv *env = get_jni_env();

	toml_table_t *tbl = miniC_open_asset(asset_path);
	if (tbl == NULL) return;

	// [options]
	toml_table_t *options = toml_table_table(tbl, "options");
	if (options != NULL) {
		toml_value_t coin_limit_v = toml_table_int(options, "coin_limit");
		if (coin_limit_v.ok) {
			miniCL_set_default(coin_limit_v.u.i & 0xFFFF);
		}

		toml_value_t too_rich_v = toml_table_int(options, "too_rich_threshold");
		if (too_rich_v.ok) {
			miniCL_set_too_rich(too_rich_v.u.i & 0xFFFF);
		}

		// TODO: Engine Speed?
	}

	// [armor_models]
	toml_table_t *models = toml_table_table(tbl, "armor_models");
	if (models != NULL) {
		int num_fields = toml_table_len(models);
		for (int i = 0; i < num_fields; i++) {
			int field_len;
			const char *item = toml_table_key(models, i, &field_len);

			toml_value_t model = toml_table_string(models, item);
			if (model.ok) {
				if (*item == '\0')
					miniAM_set_default_model(model.u.s);
				else
					miniAM_add_model(item, model.u.s);
				free(model.u.s);
			} else {
				LOGD("Invalid Armor Model entry: %s", item);
			}
		}
	}

	toml_free(tbl);
}
