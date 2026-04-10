#include "backend.h"

#include <errno.h>

MiniFILE *assets_mkstemp(char *_Nonnull path_template) {
	/* Assets are readonly. */
	errno = EROFS;
	return NULL;
}
