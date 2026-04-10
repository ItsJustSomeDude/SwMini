#ifndef NEEDLE_FILES_H
#define NEEDLE_FILES_H

#include <sys/types.h>

int mkdir_p(const char *path, mode_t mode);

void format_bytes(char *buf, size_t bufsize, size_t bytes);

#endif //NEEDLE_FILES_H
