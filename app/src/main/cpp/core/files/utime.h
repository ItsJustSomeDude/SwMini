#ifndef NEEDLE_FILES_UTIME_H
#define NEEDLE_FILES_UTIME_H

#include "core/impl_files/backend.h"
#include <utime.h>

typedef struct mini_utimbuf {
	__kernel_old_time_t actime;
	__kernel_old_time_t modtime;
} mini_utimbuf;

IMPL_BACKEND_BODY_UTIME_(, miniF_);

#endif /* NEEDLE_FILES_UTIME_H */
