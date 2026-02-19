#ifndef NEEDLE_CSTRINGS_H
#define NEEDLE_CSTRINGS_H

#include "hooks.h"

#include <stdbool.h>

typedef char CppString;

H_DL_FUNCTION(
	create_basic_string,
	void, (CppString * *destinationPointer, const char *)
)

void init_feature_cstrings();
void destroy_feature_cstrings();

/** Returns true if a workaround was applied. */
bool cstrings_apply_workarounds(const char *source, const char *replacement);

void cstrings_add_replacement(const char *source, const char *replacement);
const char *cstrings_get_replacement(const char *source);

void miniCStr_read_asset(const char *asset_path);

#endif //NEEDLE_CSTRINGS_H
