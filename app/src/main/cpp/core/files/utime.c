#include "core/files/utime.h"
#include "core/impl_files/path_parser.h"
#include "core/impl_files/errors.h"

#define LOG_TAG "MiniFilesUtime"

int miniF_utime(const char *filename, const mini_utimbuf *_Nullable times) {
	ParsedPath path;
	miniP_parse(&path, filename);

	if (miniERR_path_no_backend(LOG_TAG, &path)) return -1;
	if (miniERR_path_op_unsupported(LOG_TAG, &path, path.backend->utime, "utime")) return -1;

	return path.backend->utime(path.path, times);
}
