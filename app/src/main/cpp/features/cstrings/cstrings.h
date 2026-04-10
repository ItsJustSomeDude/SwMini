#ifndef NEEDLE_CSTRINGS_H
#define NEEDLE_CSTRINGS_H

#include "core/hooks.h"

#include <stdbool.h>

void destroy_feature_cstrings();

/** Returns true if a workaround was applied. */
bool cstrings_apply_workarounds(const char *source, const char *replacement);

void cstrings_add_replacement(const char *source, const char *replacement);
const char *cstrings_get_replacement(const char *source);

void miniCStr_read_asset(const char *asset_path);

#endif //NEEDLE_CSTRINGS_H
