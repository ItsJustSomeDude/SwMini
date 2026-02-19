#include "achievements.h"

#include "config.h"
#include "features/engine_strings.h"
#include "libs/toml/toml.h"
#include "log.h"

#include <bits/strcasecmp.h>
#include <stdlib.h>

#define LOG_TAG "MiniAchievements"

void miniACH_register() {
	const char *asset_path = "achievements.toml";
	toml_table_t *tbl = miniC_open_asset(asset_path);

	int tbl_len = toml_table_len(tbl);
	for (int i = 0; i < tbl_len; i++) {
		int key_len;
		const char *id = toml_table_key(tbl, i, &key_len);

		toml_table_t *ach = toml_table_table(tbl, id);
		if (ach == NULL) {
			LOGE("Bad achievement entry: %s", id);
			continue;
		}

		char *name = NULL;
		char *desc = NULL;
		char *counter = NULL;
		int points = -1;
		int threshold = 1;

		int num_fields = toml_table_len(ach);
		for (int j = 0; j < num_fields; j++) {
			int field_len;
			const char *field = toml_table_key(ach, j, &field_len);

			if (strcasecmp(field, "name") == 0) {
				toml_value_t v = toml_table_string(ach, field);
				if (!v.ok) {
					LOGE("Bad Name for Achievement %s", id);
					goto next_achievement;
				}
				name = v.u.s;
			} else if (desc == NULL && strcasecmp(field, "description") == 0) {
				toml_value_t v = toml_table_string(ach, field);
				if (!v.ok) {
					LOGE("Bad Description for Achievement %s", id);
					goto next_achievement;
				}
				desc = v.u.s;
			} else if (desc == NULL && strcasecmp(field, "desc") == 0) {
				toml_value_t v = toml_table_string(ach, field);
				if (!v.ok) {
					LOGE("Bad Description for Achievement %s", id);
					goto next_achievement;
				}
				desc = v.u.s;
			} else if (strcasecmp(field, "counter") == 0) {
				toml_value_t v = toml_table_string(ach, field);
				if (!v.ok) {
					LOGE(
						"Bad Counter for Achievement %s. "
						"You may omit it completely to use the ID as the counter.", id
					);
					goto next_achievement;
				}
				counter = v.u.s;
			} else if (strcasecmp(field, "points") == 0) {
				toml_value_t v = toml_table_int(ach, field);
				if (!v.ok) {
					LOGE("Bad Points value for Achievement %s", id);
					goto next_achievement;
				}
				points = (int) v.u.i;
			} else if (strcasecmp(field, "pts") == 0) {
				toml_value_t v = toml_table_int(ach, field);
				if (!v.ok) {
					LOGE("Bad Points value for Achievement %s", id);
					goto next_achievement;
				}
				points = (int) v.u.i;
			} else if (strcasecmp(field, "threshold") == 0) {
				toml_value_t v = toml_table_int(ach, field);
				if (!v.ok) {
					LOGE(
						"Bad Counter for Achievement %s. "
						"You may omit it to use the default of 1", id
					);
					goto next_achievement;
				}
				threshold = (int) v.u.i;
			} else {
				LOGW("Extraneous field %s in achievement %s", field, id);
			}
		}

		if (name == NULL) {
			LOGE("Missing Name for Achievement %s", id);
			goto next_achievement;
		}
		if (desc == NULL) {
			LOGE("Missing Description for Achievement %s", id);
			goto next_achievement;
		}
		if (points == -1) {
			LOGE("Missing Points value for Achievement %s", id);
			goto next_achievement;
		}

		utf8_to_byte_sequence(name, 0, NULL);
		utf8_to_byte_sequence(desc, 0, NULL);

		create_achievement(id, name, desc, points, counter, threshold);

		next_achievement:;
		free(name);
		free(desc);
		free(counter);
	}
}
