#ifndef NEEDLE_FILES_DIRENT_H
#define NEEDLE_FILES_DIRENT_H

#include <sys/types.h>

typedef struct mini_dirent {
	char d_name[256];
} mini_dirent;

#include "core/impl_files/backend.h"

IMPL_BACKEND_BODY_DIRENT_(, miniF_);

#endif /* NEEDLE_FILES_DIRENT_H */
