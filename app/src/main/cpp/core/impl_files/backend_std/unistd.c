#include "backend.h"

#include "core/log.h"
#include "core/impl_files/path_parser.h"

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TAG "MiniUnistd"

int std_chdir(const char *path) {
	LOGW("Not changing dir.");
	errno = ENOTSUP;
	return -1;
}

char *_Nullable std_getcwd(char *_Nullable buf, size_t size) {
	LOGW("Not changing dir.");
	errno = ENOTSUP;
	return NULL;
}

ssize_t std_readlink(const char *path_string, char *_Nonnull buf, size_t buf_size) {
	ssize_t bytes_written = readlink(path_string, buf, buf_size);
	if (bytes_written < 0) {
		/* readlink errored. Clear the buffer. */
		LOGD("readlink: error.");
		buf[0] = '\0';
		return -1;
	}

	if (bytes_written == buf_size) {
		/* Possibly truncated, so cut it off to avoid common leaks. */
		LOGD("readlink: possibly truncated.");
		buf[0] = '\0';
		return bytes_written;
	}

	/* If we get here, there's at least 1 free byte in buffer. */
	buf[bytes_written + 1] = '\0';

	/* Create a copy of the string, so we can put the new one in its place. */
	char *src_copy = strdup(buf);
	if (src_copy == NULL) {
		/* Mem allocation error. Clear buffer, return -1. */
		LOGD("readlink: allocation error.");
		errno = ENOMEM;
		buf[0] = '\0';
		return -1;
	}

	miniP_from_real(buf, buf_size, src_copy);

	LOGD("readlink: Converted '%s' -> '%s'", src_copy, buf);
	free(src_copy);

	return bytes_written;
}
