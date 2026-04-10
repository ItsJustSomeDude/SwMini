#ifndef NEEDLE_FILES_IMPL_ERRORS_H
#define NEEDLE_FILES_IMPL_ERRORS_H

#include <errno.h>
#include "core/log.h"
#include "path_parser.h"

static inline int miniERR_path_no_backend(
	const char *_Nullable LOG_TAG, ParsedPath *_Nonnull path
) {
	if (path->backend == NULL) {
		if (LOG_TAG != NULL)
			LOGW("Unknown File Location: %s", path->path);
		errno = ENOENT;
		return 1;
	}
	return 0;
}

static inline int miniERR_path_op_unsupported(
	const char *_Nonnull LOG_TAG,
	ParsedPath *_Nonnull path,
	void *_Nullable func,
	const char *_Nonnull func_name
) {
	if (func == NULL) {
		LOGW("Unable to %s path '%s': Operation Not Supported.", func_name, path->path);
		errno = EOPNOTSUPP;
		return 1;
	}

	return 0;
}

static inline int miniERR_file_op_unsupported(
	const char *_Nonnull LOG_TAG,
	MiniFILE *_Nonnull file,
	void *_Nullable func,
	const char *_Nonnull func_name
) {
	if (func == NULL) {
		LOGW("Unable to %s file: Operation Not Supported.", func_name);
		errno = EOPNOTSUPP;
		return 1;
	}

	return 0;
}

static inline void miniERR_unsupported(
	const char *_Nonnull LOG_TAG, const char *_Nonnull action, const char *_Nullable extra
) {
	LOGW("Unsupported Action: %s: %s", action, extra ? extra : "(null)");
	errno = ENOTSUP;
}

#endif /* NEEDLE_FILES_IMPL_ERRORS_H */
