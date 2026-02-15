#ifndef NEEDLE_CONFIG_H
#define NEEDLE_CONFIG_H

#include "libs/toml/toml.h"

toml_table_t *miniC_open_asset(const char *asset_path);

toml_table_t *miniC_open_file(const char *file_mini_path);

toml_table_t *miniC_open_config(const char *filename);

#endif //NEEDLE_CONFIG_H
