#include "backend.h"

#include "core/log.h"
#include "core/assets.h"
#include "core/impl_files/mini_file.h"

#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <android/asset_manager.h>

#define LOG_TAG "MiniAssetsStdio"

int assets_remove(const char *path_string) {
	/* Assets are readonly. */
	errno = EROFS;
	return -1;
}

int assets_rename(const char *old_path_string, const char *new_path_string) {
	/* Assets are readonly. */
	errno = EROFS;
	return -1;
}

MiniFILE *_Nullable assets_fopen(const char *path_string, const char *_Nonnull mode) {
	if (!mode || mode[0] == '\0') {
		errno = EINVAL; /* Invalid Argument. */
		return NULL;
	}
	if (strchr(mode, 'w') || strchr(mode, 'a') || strchr(mode, '+')) {
		errno = EROFS; /* Attempted to write asset, throw Read-only FS */
		return NULL;
	}

	MiniFILE *out = malloc(sizeof(MiniFILE));
	if (out == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	AAsset *a = AAssetManager_open(g_asset_mgr, path_string, AASSET_MODE_UNKNOWN);
	if (a == NULL) {
		LOGE("Failed to open asset '%s' (%p)", path_string, g_asset_mgr);
		errno = ENOENT;
		free(out);
		return NULL;
	}

	out->backend = &backend_assets;
	out->assets_file = a;
	out->assets_eof = false;
	out->assets_err = 0;

	return out;
}

int assets_fclose(MiniFILE *_Nonnull fp) {
	AAsset_close(fp->assets_file);
	free(fp);
	return 0;
}

size_t assets_fread(void *_Nonnull buf, size_t size, size_t count, MiniFILE *_Nonnull fp) {
//	LOGD("Will now read into %p %li * %li = %li bytes from a MiniFile.",
//	     buf, size, count, size * count);

	int bytes_read = AAsset_read(fp->assets_file, buf, count * size);
	if (bytes_read < 0) {
		errno = EIO;
		fp->assets_err = true;
		LOGE("Asset Read Error: asset %p: {asset: %p, eof: %b, err: %i}, buf: %p, size: %zu",
		     fp, fp->assets_file, fp->assets_eof, fp->assets_err,
		     buf, count * size);
		return EOF; /* Read failed. */
	} else if (bytes_read < count || bytes_read == 0) {
		/* Read to the end of the file, so set EOF flag. */
		fp->assets_eof = true;
	}

//	LOGD("ASSET: Read %i bytes, is file now eof? %b", bytes_read, fp->asset_eof);

	return bytes_read;
}

long assets_ftell(MiniFILE *_Nonnull fp) {
	off_t new_pos = AAsset_seek(fp->assets_file, 0, SEEK_CUR);
	if (new_pos == -1) {
		errno = EIO;
		fp->assets_err = true;
	}
	return new_pos;
}

int assets_fseek(MiniFILE *_Nonnull fp, long offset, int whence) {
	off_t new_pos = AAsset_seek(fp->assets_file, offset, whence);
	if (new_pos == -1) {
		errno = EIO;
		fp->assets_err = true;
		return -1;
	}
	fp->assets_eof = false;
	return 0;
}

int assets_fgetc(MiniFILE *_Nonnull fp) {
	unsigned char byte;
	int bytes_read = AAsset_read(fp->assets_file, &byte, 1);
	if (bytes_read == 0) { /* Read EOF */
		fp->assets_eof = true;
		return EOF;
	} else if (bytes_read < 0) { /* Read Err */
		fp->assets_err = true;
		errno = EIO;
		return EOF;
	}

	return (int) byte;
}

int assets_getc(MiniFILE *_Nonnull fp) {
	return assets_fgetc(fp);
}

int assets_ferror(MiniFILE *_Nonnull fp) {
	return fp->assets_err;
}

void assets_clearerr(MiniFILE *_Nonnull fp) {
	fp->assets_eof = false;
	fp->assets_err = false;
}

__nodiscard int assets_feof(MiniFILE *_Nonnull fp) {
	return fp->assets_eof;
}

char *_Nullable assets_fgets(char *_Nonnull buf, int size, MiniFILE *_Nonnull fp) {
	if (fp->assets_eof)
		return NULL; /* Already at EOF, don't set errno. */

	size_t bytes_read = 0;
	char c;
	while (bytes_read < size - 1) {
		/* Read single byte at a time. */
		int status = AAsset_read(fp->assets_file, &c, 1);
		if (status < 0) { /* Error */
			errno = EIO;
			return NULL;
		} else if (status == 0 && bytes_read == 0) {
			/* EOF immediately. return NULL */
			fp->assets_eof = true;
			return NULL;
		} else if (status == 0) {
			/* EOF. Set eof, Terminate, but don't set errno. */
			fp->assets_eof = true;
			break;
		}

		buf[bytes_read] = c;
		bytes_read++;
		if (c == '\n') break;
	}
	buf[bytes_read] = '\0';
	return buf;
}

int assets_fscanf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int result = assets_vfscanf(fp, fmt, ap);
	va_end(ap);
	return result;
}

int assets_vfscanf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, va_list args) {
	LOGW("Cannot fscanf from assets.");
	errno = ENOTSUP;
	return -1;
}

size_t assets_fwrite(const void *_Nonnull buf, size_t size, size_t count, MiniFILE *_Nonnull fp) {
	LOGW("Cannot write Asset file.");
	errno = EROFS;
	return 0;
}

int assets_vfprintf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, va_list args) {
	LOGW("Cannot write Asset file.");
	errno = EROFS;
	return 0;
}

int assets_fprintf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int result = assets_vfprintf(fp, fmt, ap);
	va_end(ap);
	return result;
}

int assets_fflush(MiniFILE *_Nullable fp) {
	LOGW("Cannot flush Asset file.");
	errno = EROFS;
	return -1;
}

int assets_fcntl(int fd, int op, ...) {
	LOGW("Cannot fnctl Asset file.");
	errno = ENOTSUP;
	return -1;
}

int assets_fileno(MiniFILE *_Nonnull fp) {
	LOGW("Attempted to get file descriptor of Asset file.");
	errno = ENOTSUP;
	return -1;
}

int assets_setvbuf(MiniFILE *_Nonnull fp, char *_Nullable buf, int mode, size_t size) {
	LOGW("Cannot change buffering of Asset file.");
	errno = EROFS;
	return -1;
}
