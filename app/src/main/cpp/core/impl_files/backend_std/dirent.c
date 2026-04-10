#include "backend.h"

#include "core/impl_files/mini_file.h"
#include <malloc.h>
#include <errno.h>
#include <dirent.h>

#define LOG_TAG "MiniDirent"

MiniDIR *_Nullable std_opendir(const char *path_string) {
	DIR *dir = opendir(path_string);
	if (dir == NULL)
		return NULL;

	MiniDIR *out = malloc(sizeof(MiniDIR));
	if (out == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	out->std_dir = dir;
	out->backend = &backend_std;

	return out;
}

mini_dirent *_Nullable std_readdir(MiniDIR *_Nonnull dir) {

	return readdir(dir->std_dir);
}

int std_closedir(MiniDIR *_Nonnull dir) {
	int r = closedir(dir->std_dir);
	free(dir);
	return r;
}
