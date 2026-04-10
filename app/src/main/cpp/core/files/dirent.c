#include "core/files/dirent.h"

#include "core/log.h"
#include "core/impl_files/errors.h"
#include "core/impl_files/path_parser.h"
#include "core/impl_files/mini_file.h"

#define LOG_TAG "MiniFilesDirent"

MiniDIR *_Nullable miniF_opendir(const char *path_string) {
	ParsedPath path;
	miniP_parse(&path, path_string);

	if (miniERR_path_no_backend(LOG_TAG, &path)) return NULL;
	if (miniERR_path_op_unsupported(LOG_TAG, &path, path.backend->opendir, "opendir"))
		return NULL;

	return path.backend->opendir(path_string);
}

mini_dirent *_Nullable miniF_readdir(MiniDIR *_Nonnull dir) {
	return dir->backend->readdir(dir);
}

int miniF_closedir(MiniDIR *_Nonnull dir) {
	return dir->backend->closedir(dir);
}
