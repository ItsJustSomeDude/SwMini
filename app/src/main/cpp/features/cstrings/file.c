#include "config.h"
#include "cstrings.h"
#include "features/engine_strings.h"
#include "libs/toml/toml.h"
#include "log.h"

#include <stdlib.h>

#define LOG_TAG "MiniCSFile"

void miniCStr_read_asset(const char *asset_path) {
	toml_table_t *tbl = miniC_open_asset(asset_path);

	int tbl_len = toml_table_len(tbl);
	for (int i = 0; i < tbl_len; i++) {
		int key_len;
		const char *key = toml_table_key(tbl, i, &key_len);
		toml_value_t val = toml_table_string(tbl, key);

		if (!val.ok) {
			LOGE("Invalid entry in CStrings file: %s index %i", asset_path, i);
			continue;
		}

		utf8_to_byte_sequence(val.u.s, val.u.sl, NULL);

		cstrings_add_replacement(key, val.u.s);
		free(val.u.s);
	}
}
