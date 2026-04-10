#include <string.h>
#include <malloc.h>
#include "core/files/stdlib.h"
#include "core/impl_files/path_parser.h"
#include "core/impl_files/errors.h"
#include "core/impl_files/mini_file.h"

#define LOG_TAG "MiniFilesStdlib"

/* `path_template` is a MiniPath. `path.path` is "real".
 * We need to overwrite the input `path_template` with the converted one. */
MiniFILE *miniF_mkstemp(char *_Nonnull path_template) {
	ParsedPath path;
	miniP_parse(&path, path_template);

	if (miniERR_path_no_backend(LOG_TAG, &path)) return NULL;
	if (miniERR_path_op_unsupported(LOG_TAG, &path, path.backend->mkstemp, "mkstemp"))
		return NULL;

	MiniFILE *ret = path.backend->mkstemp(path.path);
	if (ret == NULL) return NULL;

	/* Convert path back. For now, just std is all that's needed. */

	char out[MINI_PATH_SIZE];
	miniP_from_real(out, sizeof(out), path.path);
	strcpy(path_template, out);

	return ret;
}
