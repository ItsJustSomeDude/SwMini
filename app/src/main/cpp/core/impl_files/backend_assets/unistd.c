#include "backend.h"
#include "core/log.h"

#include <string.h>
#include <errno.h>

#define LOG_TAG "MiniUnistd"

int assets_chdir(const char *path) {
	LOGW("Not changing dir.");
	errno = ENOTSUP;
	return -1;
}

char *_Nullable assets_getcwd(char *_Nullable buf, size_t size) {
	LOGW("Not changing dir.");
	errno = ENOTSUP;
	return NULL;
}

int assets_rmdir(const char *path_string) {
	/* Assets are readonly. */
	errno = EROFS;
	return -1;
}

int assets_link(const char *old_path_string, const char *new_path_string) {
	/* Assets are readonly. */
	errno = EROFS;
	return -1;
}

int assets_symlink(const char *old_path_string, const char *new_path_string) {
	/* Assets are readonly. */
	errno = EROFS;
	return -1;
}

int assets_unlink(const char *path_string) {
	/* Assets are readonly. */
	errno = EROFS;
	return -1;
}

ssize_t assets_readlink(const char *path_string, char *_Nonnull buf, size_t buf_size) {
	/* Links don't exist in assets. */
	errno = EOPNOTSUPP;
	return -1;
}
