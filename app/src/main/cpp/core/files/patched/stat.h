#ifndef NEEDLE_PATCHED_STAT_H
#define NEEDLE_PATCHED_STAT_H

#include "core/files/stat.h"

#define mkdir(...) miniF_mkdir(__VA_ARGS__)

#define stat(...) miniF_stat(__VA_ARGS__)
#define lstat(...) miniF_lstat(__VA_ARGS__)

#endif /* NEEDLE_PATCHED_STAT_H */
