#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"
#include "tools/files.h"
#include "global.h"
#include "log.h"

#define LOG_TAG "MiniFileFunctions"

static bool nonexistent(MiniPath *p) {
	if (p->type == ERROR || p->type == UNKNOWN) {
		errno = ENOENT; /* No such file or directory. */
		return true;
	}
	return false;
}

static bool unwritable(MiniPath *p) {
	if (p->type == ERROR || p->type == UNKNOWN) {
		errno = ENOENT; /* No such file or directory. */
		return true;
	} else if (p->type == ASSET || p->type == RESOURCE) {
		errno = EROFS; /* Read-only FS, because assets and resources cannot be written. */
		return true;
	}
	return false;
}

static bool unreal(MiniPath *p, int err) {
	if (p->type == REAL) return false;
	errno = err;
	return true;
}

static void unsupported_path_type(MiniPath *path, const char *action) {
	LOGW("Attempted to %s path of unknown type %i: '%s'", action, path->type, path->path);
	errno = ENOTSUP;
}

static void unsupported_file_type(MiniFILE *file, const char *action) {
	LOGW("Attempted to %s MiniFile of unknown type %i.", action, file->type);
	errno = ENOTSUP;
}

static void unsupported_dir_type(MiniDIR *dir, const char *action) {
	LOGW("Attempted to %s MiniDir of unknown type %i.", action, dir->type);
	errno = ENOTSUP;
}


int miniP_chdir(MiniPath *path) {
	LOGW("Not changing dir.");
	errno = ENOTSUP;
	return -1;
}

char *_Nullable miniP_getcwd(char *_Nullable buf, size_t size) {
	LOGW("Not changing dir.");
	errno = ENOTSUP;
	return NULL;
}

int miniP_remove(MiniPath *path) {
	if (nonexistent(path) || unwritable(path)) return -1;

	return remove(path->path);
}

int miniP_rename(MiniPath *old_path, MiniPath *new_path) {
	if (nonexistent(old_path) || unwritable(old_path) || unwritable(new_path)) return -1;

	return rename(old_path->path, new_path->path);
}


int miniP_link(MiniPath *old_path, MiniPath *new_path) {
	if (nonexistent(old_path) || unwritable(new_path)) return -1;
	/* Cross-Device Error. Only real file can be source of link. */
	if (unreal(old_path, EXDEV)) return -1;

	return link(old_path->path, new_path->path);
}

int miniP_symlink(MiniPath *old_path, MiniPath *new_path) {
	if (nonexistent(old_path) || unwritable(new_path)) return -1;
	/* Cross-Device Error. Only real file can be source of link. */
	if (unreal(old_path, EXDEV)) return -1;

	return symlink(old_path->path, new_path->path);
}

int miniP_unlink(MiniPath *path) {
	if (nonexistent(path) || unwritable(path)) return -1;

	return unlink(path->path);
}

ssize_t miniP_readlink(MiniPath *path, char *_Nonnull buf, size_t buf_size) {
	if (nonexistent(path)) return -1;

	ssize_t bytes_written = readlink(path->path, buf, buf_size);
	if (bytes_written < 0) {
		/* readlink errored. Clear the buffer. */
		LOGD("readlink: error.");
		buf[0] = '\0';
		return -1;
	}

	if (bytes_written == buf_size) {
		/* Possibly truncated, so cut it off to avoid common leaks. */
		LOGD("readlink: possibly truncated.");
		buf[0] = '\0';
		return bytes_written;
	}

	/* If we get here, there's at least 1 free byte in buffer. */
	buf[bytes_written + 1] = '\0';

	/* Create a copy of the string, so we can put the new one in its place. */
	char *src_copy = strdup(buf);
	if (src_copy == NULL) {
		/* Mem allocation error. Clear buffer, return -1. */
		LOGD("readlink: allocation error.");
		errno = ENOMEM;
		buf[0] = '\0';
		return -1;
	}

	real_to_mini_path(buf, buf_size, src_copy);

	LOGD("readlink: Converted '%s' -> '%s'", src_copy, buf);
	free(src_copy);

	return bytes_written;
}


int miniP_stat(MiniPath *path, struct stat *_Nonnull buf) {
	/* Unreal files do not have stats. */
	if (nonexistent(path) || unreal(path, EFAULT)) return -1;

	return stat(path->path, buf);
}

int miniP_lstat(MiniPath *path, struct stat *_Nonnull buf) {
	/* Unreal files do not have stats. */
	if (nonexistent(path) || unreal(path, EFAULT)) return -1;

	return lstat(path->path, buf);
}


MiniFILE *_Nullable miniP_fopen(MiniPath *path, const char *_Nonnull mode) {
	if (nonexistent(path)) return NULL;

	if (mode[0] != '\0') {
		/* Check if file is trying to be opened in any Write or Append mode. */
		if (mode[0] == 'w' || mode[0] == 'a' || (mode[0] == 'r' && mode[1] == '+')) {
			if (unwritable(path)) return NULL;
		}
	} else {
		errno = EINVAL;
		return NULL; /* MODE is invalid, idk what POSIX says to do here, so I'm returning NULL. */
	}

	MiniFILE *out = malloc(sizeof(MiniFILE));
	if (out == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	if (path->type == ASSET) {
		AAsset *a = AAssetManager_open(g_asset_mgr, path->path, AASSET_MODE_UNKNOWN);
		if (a == NULL) {
			errno = ENOENT;
			goto fail;
		}

		out->type = ASSET;
		out->asset = a;
		out->asset_eof = false;
		out->asset_err = false;

	} else if (path->type == REAL) {
		FILE *f = fopen(path->path, mode);
		if (f == NULL) {
			errno = ENOENT;
			goto fail;
		}

		out->type = REAL;
		out->file = f;

	} else {
		unsupported_path_type(path, "open file at");
		goto fail;
	}

	return out;

	fail:
	free(out);
	return NULL;
}

int miniP_fclose(MiniFILE *_Nonnull fp) {
	if (fp->type == ASSET) {
		AAsset_close(fp->asset);
		free(fp);
		return 0;
	} else if (fp->type == REAL) {
		int r = fclose(fp->file);
		free(fp);
		return r;
	} else {
		unsupported_file_type(fp, "close");
		return -1;
	}
}

size_t miniP_fread(void *_Nonnull buf, size_t size, size_t count, MiniFILE *_Nonnull fp) {
//	LOGD("Will now read into %p %li * %li = %li bytes from a MiniFile.",
//	     buf, size, count, size * count);

	if (fp->type == ASSET) {
		int bytes_read = AAsset_read(fp->asset, buf, count * size);
		if (bytes_read < 0) {
			errno = EIO;
			fp->asset_err = true;
			return EOF; /* Read failed. */
		} else if (bytes_read < count || bytes_read == 0) {
			/* Read to the end of the file, so set EOF flag. */
			fp->asset_eof = true;
		}

//		LOGD("ASSET: Read %i bytes, is file now eof? %b", bytes_read, fp->asset_eof);

		return bytes_read;
	} else if (fp->type == REAL) {
		size_t bytes_read = fread(buf, size, count, fp->file);

//		LOGD("FILE: Read %li bytes, is file now eof? %b", bytes_read, feof(fp->file));

		return bytes_read;
	} else {
		unsupported_file_type(fp, "read from");
		return -1;
	}
}

long miniP_ftell(MiniFILE *_Nonnull fp) {
	if (fp->type == ASSET) {
		off_t new_pos = AAsset_seek(fp->asset, 0, SEEK_CUR);
		if (new_pos == -1) {
			errno = EIO;
			fp->asset_err = true;
		}
		return new_pos;
	} else if (fp->type == REAL) {
		return ftell(fp->file);
	} else {
		unsupported_file_type(fp, "tell");
		return -1;
	}
}

int miniP_fseek(MiniFILE *_Nonnull fp, long offset, int whence) {
	if (fp->type == ASSET) {
		off_t new_pos = AAsset_seek(fp->asset, offset, whence);
		if (new_pos == -1) {
			errno = EIO;
			fp->asset_err = true;
			return -1;
		}
		fp->asset_eof = false;
		return 0;
	} else if (fp->type == REAL) {
		return fseek(fp->file, offset, whence);
	} else {
		unsupported_file_type(fp, "seek");
		return -1;
	}
}

int miniP_fgetc(MiniFILE *_Nonnull fp) {
	if (fp->type == ASSET) {
		unsigned char byte;
		int bytes_read = AAsset_read(fp->asset, &byte, 1);
		if (bytes_read == 0) { /* Read EOF */
			fp->asset_eof = true;
			return EOF;
		} else if (bytes_read < 0) { /* Read Err */
			fp->asset_err = true;
			errno = EIO;
			return EOF;
		}

		return (int) byte;
	} else if (fp->type == REAL) {
		return fgetc(fp->file);
	} else {
		unsupported_file_type(fp, "read char from");
		return EOF;
	}
}

int miniP_ferror(MiniFILE *_Nonnull fp) {
	if (fp->type == ASSET) {
		return fp->asset_err;
	} else if (fp->type == REAL) {
		return ferror(fp->file);
	} else {
		unsupported_file_type(fp, "get error state of");
		return 1;
	}
}

void miniP_clearerr(MiniFILE *_Nonnull fp) {
	if (fp->type == ASSET) {
		fp->asset_eof = false;
		fp->asset_err = false;
	} else if (fp->type == REAL) {
		clearerr(fp->file);
	} else {
		unsupported_file_type(fp, "clear error state of");
	}
}

__nodiscard int miniP_feof(MiniFILE *_Nonnull fp) {
	if (fp->type == ASSET) {
		return fp->asset_eof;
	} else if (fp->type == REAL) {
		return feof(fp->file);
	} else {
		unsupported_file_type(fp, "get EOF state of");
		return 1;
	}
}

char *_Nullable miniP_fgets(char *_Nonnull buf, int size, MiniFILE *_Nonnull fp) {
	if (fp->type == ASSET) {
		if (fp->asset_eof)
			return NULL; /* Already at EOF, don't set errno. */

		size_t bytes_read = 0;
		char c;
		while (bytes_read < size - 1) {
			/* Read single byte at a time. */
			int status = AAsset_read(fp->asset, &c, 1);
			if (status < 0) { /* Error */
				errno = EIO;
				return NULL;
			} else if (status == 0 && bytes_read == 0) {
				/* EOF immediately. return NULL */
				fp->asset_eof = true;
				return NULL;
			} else if (status == 0) {
				/* EOF. Set eof, Terminate, but don't set errno. */
				fp->asset_eof = true;
				break;
			}

			buf[bytes_read] = c;
			bytes_read++;
			if (c == '\n') break;
		}
		buf[bytes_read] = '\0';
		return buf;

	} else if (fp->type == REAL) {
		return fgets(buf, size, fp->file);
	} else {
		unsupported_file_type(fp, "read line from");
		return NULL;
	}
}

int miniP_fscanf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int result = miniP_vfscanf(fp, fmt, ap);
	va_end(ap);
	return result;
}

int miniP_vfscanf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, va_list args) {
	if (fp->type == ASSET) {
		LOGW("Cannot fscanf from assets.");
		errno = ENOTSUP;
		return -1;
	} else if (fp->type == REAL) {
		return vfscanf(fp->file, fmt, args);
	} else {
		unsupported_file_type(fp, "scan from");
		return 1;
	}
}

size_t miniP_fwrite(const void *_Nonnull buf, size_t size, size_t count, MiniFILE *_Nonnull fp) {
	if (fp->type == ASSET) {
		LOGW("Cannot write Asset file.");
		errno = EROFS;
		return 0;
	} else if (fp->type == REAL) {
		return fwrite(buf, size, count, fp->file);
	} else {
		unsupported_file_type(fp, "write to");
		return 1;
	}
}

int miniP_vfprintf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, va_list args) {
	if (fp->type == ASSET) {
		LOGW("Cannot write Asset file.");
		errno = EROFS;
		return 0;
	} else if (fp->type == REAL) {
		return vfprintf(fp->file, fmt, args);
	} else {
		unsupported_file_type(fp, "print to");
		return 1;
	}
}

int miniP_fflush(MiniFILE *_Nullable fp) {
	if (fp->type == ASSET) {
		LOGW("Cannot flush Asset file.");
		errno = EROFS;
		return -1;
	} else if (fp->type == REAL) {
		return fflush(fp->file);
	} else {
		unsupported_file_type(fp, "flush");
		return 1;
	}
}


int miniP_fileno(MiniFILE *_Nonnull fp) {
	if (fp->type == ASSET) {
		LOGW("Attempted to get file descriptor of Asset file.");
		errno = ENOTSUP;
		return -1;
	}

	return fileno(fp->file);
}


int miniP_mkdir(MiniPath *path, mode_t mode) {
	if (nonexistent(path) || unwritable(path)) return -1;

	return mkdir(path->path, mode);
}

int miniP_mkdir_p(MiniPath *path, mode_t mode) {
	if (nonexistent(path) || unwritable(path)) return -1;

	return mkdir_p(path->path, mode);
}

int miniP_rmdir(MiniPath *path) {
	if (nonexistent(path) || unwritable(path)) return -1;

	return rmdir(path->path);
}

MiniDIR *_Nullable miniP_opendir(MiniPath *path) {
	if (nonexistent(path)) return NULL;

	/* It's safe to malloc and return this, because it will be freed by mini_path_closedir. */
	MiniDIR *out = malloc(sizeof(MiniDIR));
	if (out == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	if (path->type == ASSET) {
		AAssetDir *d = AAssetManager_openDir(g_asset_mgr, path->path);
		if (d == NULL) {
			errno = ENOENT;
			goto fail;
		}

		/* Create the fake structure that will be populated by reading. */
		out->asset_entry = malloc(sizeof(struct dirent));
		if (out->asset_entry == NULL) {
			errno = ENOMEM;
			goto fail;
		}
		/* Fill structure with empty data. */
		out->asset_entry->d_ino = 0;
		out->asset_entry->d_off = 0;
		out->asset_entry->d_reclen = sizeof(struct dirent);
		out->asset_entry->d_type = DT_UNKNOWN;
		out->asset_entry->d_name[0] = '\0';

		out->type = ASSET;
		out->asset_dir = d;

	} else if (path->type == REAL) {
		DIR *d = opendir(path->path);
		if (d == NULL)
			/* errno was already set by opendir. */
			goto fail;

		out->type = REAL;
		out->dir = d;
	} else {
		unsupported_path_type(path, "open directory at");
		goto fail;
	}

	return out;

	fail:
	free(out);
	return NULL;
}

struct dirent *_Nullable miniP_readdir(MiniDIR *_Nonnull dir) {
	if (dir->type == REAL) {
		struct dirent *dir_entry = readdir(dir->dir);
		if (!dir_entry)
			/* errno was already set by readdir, or left untouched if it's just the end. */
			return NULL;

		/* TODO: This probably has lots of problems... */
		const size_t out_size = sizeof(dir_entry->d_name);
		char out[out_size];
		real_to_mini_path(out, sizeof(out), dir_entry->d_name);

		LOGD("Mini Readdir says: %p %s -> %s", dir_entry->d_name, dir_entry->d_name, out);
		strncpy(dir_entry->d_name, out, out_size);

		return dir_entry;
	} else if (dir->type == ASSET) {
		const char *name = AAssetDir_getNextFileName(dir->asset_dir);
		if (name == NULL) {
			/* End of files in dir. Don't touch errno. */
			dir->asset_entry->d_name[0] = '\0';
			return NULL;
		} else {
			strncpy(dir->asset_entry->d_name, name, sizeof(dir->asset_entry->d_name));
		}
		return dir->asset_entry;
	} else {
		LOGW("Attempted to iterate directory of unknown type: %i", dir->type);
		return NULL;
	}
}

int miniP_closedir(MiniDIR *_Nonnull dir) {
	if (dir->type == REAL) {
		int r = closedir(dir->dir);
		free(dir);
		return r;

	} else if (dir->type == ASSET) {
		AAssetDir_close(dir->asset_dir);
		free(dir->asset_entry);
		free(dir);
		return 0;

	} else {
		unsupported_dir_type(dir, "close");
		return -1;
	}
}


int miniP_utime(MiniPath *filename, const struct utimbuf *_Nullable times) {
	if (nonexistent(filename) || unwritable(filename)) return -1;

	return utime(filename->path, times);
}

int miniP_setvbuf(MiniFILE *_Nonnull fp, char *_Nullable buf, int mode, size_t size) {
	if (fp->type == ASSET) {
		LOGW("Cannot change buffering of Asset file.");
		errno = EROFS;
		return -1;
	} else if (fp->type == REAL) {
		return setvbuf(fp->file, buf, mode, size);
	} else {
		unsupported_file_type(fp, "setvbuf");
		return 1;
	}
}
