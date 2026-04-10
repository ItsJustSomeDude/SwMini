#include "backend.h"

#include "core/log.h"
#include "core/impl_files/mini_file.h"

#include <stdio.h>
#include <errno.h>
#include <malloc.h>

#define LOG_TAG "MiniFileFunctions"

MiniFILE *_Nullable std_fopen(const char *path_string, const char *_Nonnull mode) {
	FILE *f = fopen(path_string, mode);
	if (f == NULL)
		return NULL;

	MiniFILE *out = malloc(sizeof(MiniFILE));
	if (out == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	out->std_file = f;
	out->backend = &backend_std;
	return out;
}

int std_fclose(MiniFILE *_Nonnull fp) {
	int r = fclose(fp->std_file);
	free(fp);
	return r;
}

size_t std_fread(void *_Nonnull buf, size_t size, size_t count, MiniFILE *_Nonnull fp) {
	return fread(buf, size, count, fp->std_file);
}

long std_ftell(MiniFILE *_Nonnull fp) {
	return ftell(fp->std_file);
}

int std_fseek(MiniFILE *_Nonnull fp, long offset, int whence) {
	return fseek(fp->std_file, offset, whence);
}

int std_fgetc(MiniFILE *_Nonnull fp) {
	return fgetc(fp->std_file);
}

int std_getc(MiniFILE *_Nonnull fp) {
	return getc(fp->std_file);
}

int std_ferror(MiniFILE *_Nonnull fp) {
	return ferror(fp->std_file);
}

void std_clearerr(MiniFILE *_Nonnull fp) {
	clearerr(fp->std_file);
}

__nodiscard int std_feof(MiniFILE *_Nonnull fp) {
	return feof(fp->std_file);
}

char *_Nullable std_fgets(char *_Nonnull buf, int size, MiniFILE *_Nonnull fp) {
	return fgets(buf, size, fp->std_file);
}

int std_fscanf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int result = std_vfscanf(fp, fmt, ap);
	va_end(ap);
	return result;
}

int std_vfscanf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, va_list args) {
	return vfscanf(fp->std_file, fmt, args);
}

size_t std_fwrite(const void *_Nonnull buf, size_t size, size_t count, MiniFILE *_Nonnull fp) {
	return fwrite(buf, size, count, fp->std_file);
}

int std_vfprintf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, va_list args) {
	return vfprintf(fp->std_file, fmt, args);
}

int std_fprintf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int result = std_vfprintf(fp, fmt, ap);
	va_end(ap);
	return result;
}

int std_fflush(MiniFILE *_Nullable fp) {
	return fflush(fp->std_file);
}

int std_fcntl(int fd, int op, ...) {
	LOGW("Cannot fnctl Std file.");
	errno = ENOTSUP;
	return -1;
}

int std_fileno(MiniFILE *_Nonnull fp) {
	return fileno(fp->std_file);
}

int std_setvbuf(MiniFILE *_Nonnull fp, char *_Nullable buf, int mode, size_t size) {
	return setvbuf(fp->std_file, buf, mode, size);
}
