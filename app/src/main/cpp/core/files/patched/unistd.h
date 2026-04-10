#ifndef NEEDLE_PATCHED_UNISTD_H
#define NEEDLE_PATCHED_UNISTD_H

#include "core/files/unistd.h"

#define chdir(...) miniF_chdir(__VA_ARGS__)
#define getcwd(...) miniF_getcwd(__VA_ARGS__)

#define rmdir(...) miniF_rmdir(__VA_ARGS__)

#define link(...) miniF_link(__VA_ARGS__)
#define symlink(...) miniF_symlink(__VA_ARGS__)
#define unlink(...) miniF_unlink(__VA_ARGS__)
#define readlink(...) miniF_readlink(__VA_ARGS__)

#endif /* NEEDLE_PATCHED_UNISTD_H */
