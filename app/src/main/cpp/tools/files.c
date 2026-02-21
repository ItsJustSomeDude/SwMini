#include "files.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>


int mkdir_p(const char *path, mode_t mode) {
	char tmp[1024];
	char *p = NULL;
	size_t len;

	snprintf(tmp, sizeof(tmp), "%s", path);
	len = strlen(tmp);

	if (tmp[len - 1] == '/')
		tmp[len - 1] = '\0';

	for (p = tmp + 1; *p; p++) {
		if (*p == '/') {
			*p = '\0';
			if (mkdir(tmp, mode) && errno != EEXIST)
				return -1;
			*p = '/';
		}
	}

	if (mkdir(tmp, mode) && errno != EEXIST)
		return -1;

	return 0;
}

void format_bytes(char *buf, size_t bufsize, size_t bytes) {
	static const char *units[] = {"B", "KB", "MB", "GB", "TB"};
	double val = (double) bytes;
	int unit = 0;

	while (val >= 1024 && unit < 4) {
		val /= 1024;
		unit++;
	}

	snprintf(buf, bufsize, "%.1f %s", val, units[unit]);
}
