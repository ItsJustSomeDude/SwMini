#ifndef NEEDLE_PATH_PARSER_H
#define NEEDLE_PATH_PARSER_H

#include "constants.h"
#include "core/impl_files/backend.h"

typedef struct ParsedPath {
	/* The backend that can be used with this parsed path.
	 * If NULL, the path must not have been valid. */
	MiniFileBackend *_Nullable backend;

	/* The path, converted to a state that can be used with the given backend. */
	char path[MINI_PATH_SIZE];
} ParsedPath;

void miniP_parse(ParsedPath *_Nonnull out, const char *_Nonnull path);

void miniP_from_real(char *_Nonnull out, size_t out_size, const char *_Nullable real_path);

_Bool miniP_simplify(char *_Nonnull out, const char *_Nonnull path);

#endif /* NEEDLE_PATH_PARSER_H */
