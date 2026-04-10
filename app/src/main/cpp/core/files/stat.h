#ifndef NEEDLE_FILES_STAT_H
#define NEEDLE_FILES_STAT_H

#include <sys/types.h>
#include "core/impl_files/backend.h"

typedef struct mini_timespec {
	/** Number of seconds. */
	time_t tv_sec;
	/** Number of nanoseconds. Must be less than 1,000,000,000. */
	long tv_nsec;
} mini_timespec;

typedef struct mini_stat {
	dev_t st_dev;
	ino_t st_ino;
	mode_t st_mode;
	nlink_t st_nlink;
	uid_t st_uid;
	gid_t st_gid;
	dev_t st_rdev;
	off_t st_size;
	int st_blksize;
	long st_blocks;

	mini_timespec st_atim;
	mini_timespec st_mtim;
	mini_timespec st_ctim;
} mini_stat;

IMPL_BACKEND_BODY_STAT_(, miniF_);

#endif /* NEEDLE_FILES_STAT_H */
