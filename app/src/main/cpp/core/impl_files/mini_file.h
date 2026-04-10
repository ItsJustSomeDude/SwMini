#ifndef NEEDLE_FILES_IMPL_MINI_FILE_H
#define NEEDLE_FILES_IMPL_MINI_FILE_H

typedef struct MiniFileBackend MiniFileBackend;
typedef struct __sFILE FILE; /* NOLINT(bugprone-reserved-identifier) */
typedef struct DIR DIR;
typedef struct AAsset AAsset;
typedef struct AAssetDir AAssetDir;

typedef struct mini_dirent mini_dirent;

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

	/* This is a union of the backing handles to every different backend's "file" object. */
	union {
		struct {
			DIR *_Nonnull std_dir;
			mini_dirent *_Nonnull std_dirent;
		};
		struct {
			AAssetDir *_Nonnull assets_dir;
			/** Most of the values are blank or fake, because it's not pointing to a real file.
			 * The `d_name` is the only thing guaranteed to exist.
			 * MUST be freed separately on dirclose! */
			mini_dirent *_Nonnull assets_dirent;
		};
	};
} MiniDIR;

#endif /* NEEDLE_FILES_IMPL_MINI_FILE_H */
