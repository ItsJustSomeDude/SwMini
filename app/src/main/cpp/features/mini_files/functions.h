#ifndef NEEDLE_FUNCTIONS_H
#define NEEDLE_FUNCTIONS_H

#include "mini_files.h"
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utime.h>
#include <stdbool.h>
#include <android/asset_manager.h>

struct stat;
struct utimbuf;

/** This is like the FILE struct, but takes Mini into account too. */
typedef struct MiniFILE {
	MiniPathType type;
	union {
		/** Only present when `type == REAL` */
		FILE *file;
		struct {
			/** Only present when `type == ASSET` */
			AAsset *asset;
			/** Only present when `type == ASSET` */
			bool asset_eof;
			/** Only present when `type == ASSET` */
			int asset_err;
		};
	};
} MiniFILE;

/** This is like the DIR struct, but takes Mini into account too. */
typedef struct MiniDIR {
	MiniPathType type;
	union {
		/** Only present when `type == REAL` */
		DIR *dir;
		struct {
			/** Only present when `type == ASSET` */
			AAssetDir *asset_dir;
			/** Only present when `type == ASSET`.
			 * Most of the values are blank or fake, because it's not pointing to a real file.
			 * The `d_name` is the only thing guaranteed to exist.
			 * MUST be freed separately on dirclose! */
			struct dirent *asset_entry;
		};
	};
} MiniDIR;

/* Here are the functions that take MiniPaths. */

int miniP_chdir(MiniPath *path);
char *_Nullable miniP_getcwd(char *_Nullable buf, size_t size);

int miniP_remove(MiniPath *path);
int miniP_rename(MiniPath *old_path, MiniPath *new_path);

int miniP_link(MiniPath *old_path, MiniPath *new_path);
int miniP_symlink(MiniPath *old_path, MiniPath *new_path);
int miniP_unlink(MiniPath *path);
ssize_t miniP_readlink(MiniPath *path, char *_Nonnull buf, size_t buf_size);

int miniP_stat(MiniPath *path, struct stat *_Nonnull buf);
int miniP_lstat(MiniPath *path, struct stat *_Nonnull buf);

MiniFILE *_Nullable miniP_fopen(MiniPath *path, const char *_Nonnull mode);
int miniP_fclose(MiniFILE *_Nonnull fp);
size_t miniP_fread(void *_Nonnull buf, size_t size, size_t count, MiniFILE *_Nonnull fp);
__nodiscard long miniP_ftell(MiniFILE *_Nonnull fp);
int miniP_fseek(MiniFILE *_Nonnull fp, long offset, int whence);
__nodiscard int miniP_fgetc(MiniFILE *_Nonnull fp);
__nodiscard int miniP_ferror(MiniFILE *_Nonnull fp);
void miniP_clearerr(MiniFILE *_Nonnull fp);
__nodiscard int miniP_feof(MiniFILE *_Nonnull fp);
char *_Nullable miniP_fgets(char *_Nonnull buf, int size, MiniFILE *_Nonnull fp);
int miniP_fscanf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, ...) __scanflike(2, 3);
int miniP_vfscanf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, va_list args) __scanflike(2, 0);

size_t miniP_fwrite(const void *_Nonnull buf, size_t size, size_t count, MiniFILE *_Nonnull fp);
int miniP_vfprintf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, va_list va) __printflike(2, 0);
int miniP_fflush(MiniFILE *_Nullable fp);

#define miniP_getc(fp) miniP_fgetc(fp)

//int mini_path_fcntl(int fd, int op, ...);
int miniP_fileno(MiniFILE *_Nonnull fp);

int miniP_mkdir(MiniPath *path, mode_t mode);
int miniP_rmdir(MiniPath *path);
MiniDIR *_Nullable miniP_opendir(MiniPath *path);
struct dirent *_Nullable miniP_readdir(MiniDIR *_Nonnull dir);
int miniP_closedir(MiniDIR *_Nonnull dir);

int miniP_utime(MiniPath *filename, const struct utimbuf *_Nullable times);
int miniP_setvbuf(MiniFILE *_Nonnull fp, char *_Nullable buf, int mode, size_t size);

// TODO: Must be hooked: Lua's loadFile
// doFile is a macro so doesn't need to change.

#endif //NEEDLE_FUNCTIONS_H
