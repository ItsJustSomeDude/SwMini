#include "backend.h"

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "core/impl_files/mini_file.h"

MiniFILE *std_mkstemp(char *_Nonnull path_template) {
	int fd = mkstemp(path_template);
	if (fd == -1) return NULL;

	FILE *f = fdopen(fd, "r+");

	MiniFILE *m = malloc(sizeof(MiniFILE));
	if (m == NULL) {
		close(fd);
		errno = ENOMEM;
		return NULL;
	}

	m->backend = &backend_std;
	m->std_file = f;
	
	return m;
}
