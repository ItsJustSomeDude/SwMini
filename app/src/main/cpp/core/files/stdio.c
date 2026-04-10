#include "core/files/stdio.h"

#include "core/log.h"
#include "core/impl_files/path_parser.h"
#include "core/impl_files/errors.h"
#include "core/impl_files/mini_file.h"
#include "stdlib.h"

#define LOG_TAG "MiniFilesStdio"

int miniF_remove(const char *path_string) {
	ParsedPath path;
	miniP_parse(&path, path_string);

	if (miniERR_path_no_backend(LOG_TAG, &path)) return -1;
	if (miniERR_path_op_unsupported(LOG_TAG, &path, path.backend->remove, "remove")) return -1;

	return path.backend->remove(path.path);
}

int miniF_rename(const char *old_path_string, const char *new_path_string) {
	ParsedPath old_path, new_path;
	miniP_parse(&old_path, old_path_string);
	miniP_parse(&new_path, new_path_string);

	if (miniERR_path_no_backend(LOG_TAG, &old_path)) return -1;
	if (miniERR_path_op_unsupported(LOG_TAG, &old_path, old_path.backend->rename, "rename"))
		return -1;

	return old_path.backend->rename(old_path.path, new_path.path);
}

MiniFILE *_Nullable miniF_fopen(const char *path_string, const char *_Nonnull mode) {
	ParsedPath path;
	miniP_parse(&path, path_string);

	if (miniERR_path_no_backend(LOG_TAG, &path)) return NULL;
	if (miniERR_path_op_unsupported(LOG_TAG, &path, path.backend->fopen, "fopen")) return NULL;

	return path.backend->fopen(path.path, mode);
}

int miniF_fclose(MiniFILE *_Nonnull fp) {
	if (miniERR_file_op_unsupported(LOG_TAG, fp, fp->backend->fclose, "fclose"))
		return -1;

	return fp->backend->fclose(fp);
}

size_t miniF_fread(void *_Nonnull buf, size_t size, size_t count, MiniFILE *_Nonnull fp) {
	if (miniERR_file_op_unsupported(LOG_TAG, fp, fp->backend->fread, "fread"))
		return -1;

	return fp->backend->fread(buf, size, count, fp);
}

long miniF_ftell(MiniFILE *_Nonnull fp) {
	if (miniERR_file_op_unsupported(LOG_TAG, fp, fp->backend->ftell, "ftell"))
		return -1;

	return fp->backend->ftell(fp);
}

int miniF_fseek(MiniFILE *_Nonnull fp, long offset, int whence) {
	if (miniERR_file_op_unsupported(LOG_TAG, fp, fp->backend->fseek, "fseek"))
		return -1;

	return fp->backend->fseek(fp, offset, whence);
}

int miniF_fgetc(MiniFILE *_Nonnull fp) {
	if (miniERR_file_op_unsupported(LOG_TAG, fp, fp->backend->fgetc, "fgetc"))
		return -1;

	return fp->backend->fgetc(fp);
}

int miniF_getc(MiniFILE *_Nonnull fp) {
	if (miniERR_file_op_unsupported(LOG_TAG, fp, fp->backend->getc, "getc"))
		return -1;

	return fp->backend->getc(fp);
}

int miniF_ferror(MiniFILE *_Nonnull fp) {
	if (miniERR_file_op_unsupported(LOG_TAG, fp, fp->backend->ferror, "ferror"))
		return -1;

	return fp->backend->ferror(fp);
}

void miniF_clearerr(MiniFILE *_Nonnull fp) {
	if (miniERR_file_op_unsupported(LOG_TAG, fp, fp->backend->clearerr, "clearerr"))
		return;

	return fp->backend->clearerr(fp);
}

__nodiscard int miniF_feof(MiniFILE *_Nonnull fp) {
	if (miniERR_file_op_unsupported(LOG_TAG, fp, fp->backend->feof, "feof"))
		return -1;

	return fp->backend->feof(fp);
}

char *_Nullable miniF_fgets(char *_Nonnull buf, int size, MiniFILE *_Nonnull fp) {
	if (miniERR_file_op_unsupported(LOG_TAG, fp, fp->backend->fgets, "fgets"))
		return NULL;

	return fp->backend->fgets(buf, size, fp);
}

int miniF_fscanf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int result = miniF_vfscanf(fp, fmt, ap);
	va_end(ap);
	return result;
}

int miniF_vfscanf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, va_list args) {
	if (miniERR_file_op_unsupported(LOG_TAG, fp, fp->backend->vfscanf, "vfscanf"))
		return -1;

	return fp->backend->vfscanf(fp, fmt, args);
}

size_t miniF_fwrite(const void *_Nonnull buf, size_t size, size_t count, MiniFILE *_Nonnull fp) {
	if (miniERR_file_op_unsupported(LOG_TAG, fp, fp->backend->fwrite, "fwrite"))
		return -1;

	return fp->backend->fwrite(buf, size, count, fp);
}

int miniF_vfprintf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, va_list args) {
	if (miniERR_file_op_unsupported(LOG_TAG, fp, fp->backend->vfprintf, "vfprintf"))
		return -1;

	return fp->backend->vfprintf(fp, fmt, args);
}

int miniF_fprintf(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int result = miniF_vfprintf(fp, fmt, ap);
	va_end(ap);
	return result;
}

int miniF_fflush(MiniFILE *_Nullable fp) {
	if (miniERR_file_op_unsupported(LOG_TAG, fp, fp->backend->fflush, "fflush"))
		return -1;

	return fp->backend->fflush(fp);
}

int miniF_fileno(MiniFILE *_Nonnull fp) {
	if (miniERR_file_op_unsupported(LOG_TAG, fp, fp->backend->fileno, "fileno"))
		return -1;

	return fp->backend->fileno(fp);
}

int miniF_setvbuf(MiniFILE *_Nonnull fp, char *_Nullable buf, int mode, size_t size) {
	if (miniERR_file_op_unsupported(LOG_TAG, fp, fp->backend->setvbuf, "setvbuf"))
		return -1;

	return fp->backend->setvbuf(fp, buf, mode, size);
}

MiniFILE *_Nullable miniF_tmpfile() {
	/* Arg must be writable memory. */
	char tmp[32] = "/Cache/mini_XXXXXX";

	return miniF_mkstemp(tmp);
}
