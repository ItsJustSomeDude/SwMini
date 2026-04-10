#include "core/files/unistd.h"
#include "core/log.h"
#include "core/impl_files/path_parser.h"
#include "core/impl_files/errors.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define LOG_TAG "MiniUnistd"

int miniF_chdir(const char *path) {
	LOGW("Not changing dir.");
	errno = ENOTSUP;
	return -1;
}

char *_Nullable miniF_getcwd(char *_Nullable buf, size_t size) {
	LOGW("Not changing dir.");
	errno = ENOTSUP;
	return NULL;
}

int miniF_rmdir(const char *path_string) {
	ParsedPath path;
	miniP_parse(&path, path_string);

	if (miniERR_path_no_backend(LOG_TAG, &path)) return -1;
	if (miniERR_path_op_unsupported(LOG_TAG, &path, path.backend->rmdir, "rmdir")) return -1;

	return path.backend->rmdir(path.path);
}

int miniF_link(const char *old_path_string, const char *new_path_string) {
	ParsedPath old_path, new_path;
	miniP_parse(&old_path, old_path_string);
	miniP_parse(&new_path, new_path_string);

	if (miniERR_path_no_backend(LOG_TAG, &old_path)) return -1;
	if (miniERR_path_op_unsupported(LOG_TAG, &old_path, old_path.backend->link, "link"))
		return -1;

	return old_path.backend->link(old_path.path, old_path.path);
}

int miniF_symlink(const char *old_path_string, const char *new_path_string) {
	ParsedPath old_path, new_path;
	miniP_parse(&old_path, old_path_string);
	miniP_parse(&new_path, new_path_string);

	if (miniERR_path_no_backend(LOG_TAG, &old_path)) return -1;
	if (miniERR_path_op_unsupported(LOG_TAG, &old_path, old_path.backend->symlink, "symlink"))
		return -1;

	return old_path.backend->symlink(old_path.path, new_path.path);
}

int miniF_unlink(const char *path_string) {
	ParsedPath path;
	miniP_parse(&path, path_string);

	if (miniERR_path_no_backend(LOG_TAG, &path)) return -1;
	if (miniERR_path_op_unsupported(LOG_TAG, &path, path.backend->unlink, "unlink")) return -1;

	return path.backend->unlink(path.path);
}


ssize_t miniF_readlink(const char *path_string, char *_Nonnull buf, size_t buf_size) {
	ParsedPath path;
	miniP_parse(&path, path_string);

	if (miniERR_path_no_backend(LOG_TAG, &path)) return -1;
	if (miniERR_path_op_unsupported(LOG_TAG, &path, path.backend->readlink, "readlink")) return -1;

	return path.backend->readlink(path.path, buf, buf_size);
}
