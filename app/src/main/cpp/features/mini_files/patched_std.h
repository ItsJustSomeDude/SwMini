#ifndef NEEDLE_PATCHED_STD_H
#define NEEDLE_PATCHED_STD_H

#include "functions.h"

MiniFILE *stdin_();
MiniFILE *stdout_();
MiniFILE *stderr_();

#undef stdin
#undef stdout
#undef stderr

#define stdin (stdin_())
#define stdout (stdout_())
#define stderr (stderr_())

#endif //NEEDLE_PATCHED_STD_H
