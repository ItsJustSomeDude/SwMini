#ifndef NEEDLE_FILES_IMPL_MINI_FILE_H
#define NEEDLE_FILES_IMPL_MINI_FILE_H

#include "core/files/dirent.h"

typedef struct MiniFileBackend MiniFileBackend;
typedef struct __sFILE FILE; /* NOLINT(bugprone-reserved-identifier) */
typedef struct DIR DIR;
typedef struct AAsset AAsset;
typedef struct AAssetDir AAssetDir;

typedef struct MiniFILE {
	MiniFileBackend *_Nonnull backend;

	union {
		FILE *_Nonnull std_file;
		struct {
			AAsset *_Nonnull assets_file;
			_Bool assets_eof;
			int assets_err;
		};
	};
} MiniFILE;

typedef struct MiniDIR {
	MiniFileBackend *_Nonnull backend;

	/** Most of the values are blank or fake, because it's not necessarily pointing to a real file.
	 * The `d_name` is the only thing guaranteed to exist. */
	mini_dirent dirent;

	/* This is a union of the backing handles to every different backend's "file" object. */
	union {
		struct {
			DIR *_Nonnull std_dir;
		};
		struct {
			AAssetDir *_Nonnull assets_dir;
		};
	};
} MiniDIR;

#endif /* NEEDLE_FILES_IMPL_MINI_FILE_H */
