#include <string.h>
#include "core/files/stat.h"

#include "core/impl_files/path_parser.h"
#include "core/impl_files/errors.h"

#define LOG_TAG "MiniFilesStat"

int miniF_stat(const char *path_string, mini_stat *_Nonnull buf) {
	memset(buf, 0, sizeof(mini_stat));

	ParsedPath path;
	miniP_parse(&path, path_string);

	/* `stat`ing non-existent files is expected, so don't log. */
	if (miniERR_path_no_backend(NULL, &path)) return -1;
	if (miniERR_path_op_unsupported(LOG_TAG, &path, path.backend->stat, "stat")) return -1;

	return path.backend->stat(path.path, buf);
}

int miniF_lstat(const char *path_string, mini_stat *_Nonnull buf) {
	memset(buf, 0, sizeof(mini_stat));

	ParsedPath path;
	miniP_parse(&path, path_string);

	if (miniERR_path_no_backend(LOG_TAG, &path)) return -1;
	if (miniERR_path_op_unsupported(LOG_TAG, &path, path.backend->lstat, "lstat")) return -1;

	return path.backend->lstat(path.path, buf);
}

int miniF_mkdir(const char *path_string, mode_t mode) {
	ParsedPath path;
	miniP_parse(&path, path_string);

	if (miniERR_path_no_backend(LOG_TAG, &path)) return -1;
	if (miniERR_path_op_unsupported(LOG_TAG, &path, path.backend->mkdir, "mkdir")) return -1;

	return path.backend->mkdir(path.path, mode);
}
