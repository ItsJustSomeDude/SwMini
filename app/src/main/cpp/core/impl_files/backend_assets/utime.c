#include "backend.h"

#include <errno.h>

int assets_utime(const char *filename, const mini_utimbuf *_Nullable times) {
	/* Assets don't have timestamps. */
	errno = EOPNOTSUPP;
	return -1;
}
