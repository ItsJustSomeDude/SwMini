#include "patched_functions.h"

#include "mini_files.h"
#include "functions.h"
#include "log.h"

#include <unistd.h>
#include <asm-generic/fcntl.h>
#include <linux/fcntl.h>

#define LOG_TAG "MiniPatchedFiles"


#define parse_path(out, path)   \
    MiniPath out;               \
    parse_mini_path(&out, path)

#define REDIRECT_PATCH(function)

int patched_chdir_(const char *_Nonnull path) {
	parse_path(p, path);
	return miniP_chdir(&p);
}

char *_Nullable patched_getcwd_(char *_Nullable buf, size_t size) {
	return miniP_getcwd(buf, size);
}

int patched_symlink_(const char *_Nonnull old_path, const char *_Nonnull new_path) {
	parse_path(old, old_path);
	parse_path(new, new_path);
	return miniP_symlink(&old, &new);
}

int patched_link_(const char *_Nonnull old_path, const char *_Nonnull new_path) {
	parse_path(old, old_path);
	parse_path(new, new_path);
	return miniP_link(&old, &new);
}

int patched_unlink_(const char *_Nonnull path) {
	parse_path(p, path);
	return miniP_unlink(&p);
}

ssize_t patched_readlink_(const char *_Nonnull path, char *_Nonnull buf, size_t buf_size) {
	parse_path(p, path);
	return miniP_readlink(&p, buf, buf_size);
}

int patched_mkdir_(const char *_Nonnull path, mode_t mode) {
	parse_path(p, path);
	LOGD("MKDIR... %s", p.path);
	return miniP_mkdir(&p, mode);
}

int patched_rmdir_(const char *_Nonnull path) {
	parse_path(p, path);
	return miniP_rmdir(&p);
}

int patched_utime_(const char *_Nonnull filename, const struct utimbuf *_Nullable times) {
	parse_path(f, filename);
	return miniP_utime(&f, times);
}

int patched_setvbuf_(FILE *_Nonnull fp, char *_Nullable buf, int mode, size_t size) {
	return miniP_setvbuf(fp, buf, mode, size);
}

FILE *_Nullable patched_fopen_(const char *_Nonnull path, const char *_Nonnull mode) {
	parse_path(p, path);
	return miniP_fopen(&p, mode);
}

int patched_fclose_(FILE *_Nonnull fp) {
	return miniP_fclose(fp);
}

size_t patched_fread_(void *_Nonnull buf, size_t size, size_t count, FILE *_Nonnull fp) {
	return miniP_fread(buf, size, count, fp);
}

long patched_ftell_(FILE *_Nonnull fp) {
	return miniP_ftell(fp);
}

int patched_fseek_(FILE *_Nonnull fp, long offset, int whence) {
	return miniP_fseek(fp, offset, whence);
}

int patched_fgetc_(FILE *_Nonnull fp) {
	return miniP_fgetc(fp);
}

int patched_ferror_(FILE *_Nonnull fp) {
	return miniP_ferror(fp);
}

void patched_clearerr_(FILE *_Nonnull fp) {
	miniP_clearerr(fp);
}

int patched_feof_(FILE *_Nonnull fp) {
	return miniP_feof(fp);
}

char *_Nullable patched_fgets_(char *_Nonnull buf, int size, FILE *_Nonnull fp) {
	return miniP_fgets(buf, size, fp);
}

int patched_fscanf_(FILE *_Nonnull fp, const char *_Nonnull fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int result = patched_vfscanf_(fp, fmt, ap);
	va_end(ap);
	return result;
}

int patched_vfscanf_(FILE *_Nonnull fp, const char *_Nonnull fmt, va_list args) {
	return miniP_vfscanf(fp, fmt, args);
}

int patched_fflush_(FILE *_Nullable fp) {
	return miniP_fflush(fp);
}

size_t patched_fwrite_(const void *_Nonnull buf, size_t size, size_t count, FILE *_Nonnull fp) {
	return miniP_fwrite(buf, size, count, fp);
}

int patched_fprintf_(FILE *_Nonnull fp, const char *_Nonnull fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int result = miniP_vfprintf(fp, fmt, ap);
	va_end(ap);
	return result;
}

DIR *_Nullable patched_opendir_(const char *_Nonnull path) {
	parse_path(p, path);
	return miniP_opendir(&p);
}

struct dirent *_Nullable patched_readdir_(DIR *_Nonnull dir) {
	return miniP_readdir(dir);
}

int patched_closedir_(DIR *_Nonnull dir) {
	return miniP_closedir(dir);
}

int patched_remove_(const char *_Nonnull path) {
	parse_path(p, path);
	return miniP_remove(&p);
}

int patched_rename_(const char *_Nonnull old_path, const char *_Nonnull new_path) {
	parse_path(o, old_path);
	parse_path(n, new_path);
	return miniP_rename(&o, &n);
}

int patched_lstat_(const char *_Nonnull path, struct stat *_Nonnull buf) {
	parse_path(p, path);
	return miniP_lstat(&p, buf);
}

int patched_stat_(const char *_Nonnull path, struct stat *_Nonnull buf) {
	parse_path(p, path);
	return miniP_stat(&p, buf);
}

int patched_fileno_(FILE *_Nonnull fp) {
	return miniP_fileno(fp);
}
