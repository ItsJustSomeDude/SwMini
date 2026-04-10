#ifndef NEEDLE_PATCHED_STDIO_FILES_H
#define NEEDLE_PATCHED_STDIO_FILES_H

#include "core/files/stdio_files.h"

/* Replace Standard IO Streams. */

#undef stdin
#undef stdout
#undef stderr

#define stdin miniF_stdin
#define stdout miniF_stdout
#define stderr miniF_stderr

#endif /* NEEDLE_PATCHED_STDIO_FILES_H */
