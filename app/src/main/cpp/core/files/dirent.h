#ifndef NEEDLE_FILES_DIRENT_H
#define NEEDLE_FILES_DIRENT_H

#include <sys/types.h>

#if defined(__LP64__)
#define DIRENT64_INO_T ino_t
#else
#define DIRENT64_INO_T uint64_t /* Historical accident. */
#endif

typedef struct mini_dirent {
	DIRENT64_INO_T d_ino;
	off64_t d_off;
	unsigned short d_reclen;
	unsigned char d_type;
	char d_name[256];
} mini_dirent;

#include "core/impl_files/backend.h"

IMPL_BACKEND_BODY_DIRENT_(, miniF_);

#endif /* NEEDLE_FILES_DIRENT_H */
