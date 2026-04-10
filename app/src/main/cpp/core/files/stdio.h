#ifndef NEEDLE_FILES_STDIO_H
#define NEEDLE_FILES_STDIO_H

#include "core/impl_files/backend.h"

IMPL_BACKEND_BODY_STDIO_(, miniF_);

/* Special - This one does not have args, so no backend. */

__nodiscard MiniFILE *_Nullable (miniF_tmpfile)(void);

#endif /* NEEDLE_FILES_STDIO_H */
