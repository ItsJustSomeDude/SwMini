#ifndef NEEDLE_PATCHED_DIRENT_H
#define NEEDLE_PATCHED_DIRENT_H

#include "core/files/dirent.h"

#define DIR MiniDIR

#define opendir(...) miniF_opendir(__VA_ARGS__)
#define readdir(...) miniF_readdir(__VA_ARGS__)
#define closedir(...) miniF_closedir(__VA_ARGS__)

#endif /* NEEDLE_PATCHED_DIRENT_H */
