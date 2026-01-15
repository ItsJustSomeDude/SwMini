
#ifndef NEEDLE_CSTRINGS_H
#define NEEDLE_CSTRINGS_H

#include <stdbool.h>

void init_feature_cstrings();
void destroy_feature_cstrings();

/** Returns true if a workaround was applied. */
bool cstrings_apply_workarounds(const char *source, const char *replacement);

void cstrings_add_replacement(const char *source, const char *replacement);
const char *cstrings_get_replacement(const char *source);

#endif //NEEDLE_CSTRINGS_H
