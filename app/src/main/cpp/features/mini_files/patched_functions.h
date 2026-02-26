#ifndef NEEDLE_PATCHED_FUNCTIONS_H
#define NEEDLE_PATCHED_FUNCTIONS_H

#include <stddef.h>
#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>
#include "functions.h"

#define FILE MiniFILE
#define DIR MiniDIR

#define chdir(...) patched_chdir_(__VA_ARGS__)
#define getcwd(...) patched_getcwd_(__VA_ARGS__)

#define remove(...) patched_remove_(__VA_ARGS__)
#define rename(...) patched_rename_(__VA_ARGS__)

#define link(...) patched_link_(__VA_ARGS__)
#define symlink(...) patched_symlink_(__VA_ARGS__)
#define unlink(...) patched_unlink_(__VA_ARGS__)
#define readlink(...) patched_readlink_(__VA_ARGS__)

/* `stat` is renamed to `mstat` to not step on the `stat` struct. */
#define stat(...) patched_stat_(__VA_ARGS__)
#define lstat(...) patched_lstat_(__VA_ARGS__)

#define fopen(...) patched_fopen_(__VA_ARGS__)
#define fclose(...) patched_fclose_(__VA_ARGS__)
#define fread(...) patched_fread_(__VA_ARGS__)
#define ftell(...) patched_ftell_(__VA_ARGS__)
#define fseek(...) patched_fseek_(__VA_ARGS__)
#define fgetc(...) patched_fgetc_(__VA_ARGS__)
#define ferror(...) patched_ferror_(__VA_ARGS__)
#define clearerr(...) patched_clearerr_(__VA_ARGS__)
#define feof(...) patched_feof_(__VA_ARGS__)
#define fgets(...) patched_fgets_(__VA_ARGS__)
#define fscanf(...) patched_fscanf_(__VA_ARGS__)
#define fwrite(...) patched_fwrite_(__VA_ARGS__)
#define fprintf(...) patched_fprintf_(__VA_ARGS__)
#define fflush(...) patched_fflush_(__VA_ARGS__)

#define getc(...) patched_fgetc_(__VA_ARGS__)

/* I can't really patch this. LuaFS uses it, but it should fail because `fileno` fails on Assets. */
//#define fcntl(...) patched_fcntl_(__VA_ARGS__)
#define fileno(...) patched_fileno_(__VA_ARGS__)

#define mkdir(...) patched_mkdir_(__VA_ARGS__)
#define rmdir(...) patched_rmdir_(__VA_ARGS__)
#define opendir(...) patched_opendir_(__VA_ARGS__)
#define readdir(...) patched_readdir_(__VA_ARGS__)
#define closedir(...) patched_closedir_(__VA_ARGS__)

#define utime(...) patched_utime_(__VA_ARGS__)
#define setvbuf(...) patched_setvbuf_(__VA_ARGS__)

struct utimbuf;
struct stat;

int patched_chdir_(const char *_Nonnull path);
char *_Nullable patched_getcwd_(char *_Nullable buf, size_t size);

int patched_remove_(const char *_Nonnull path);
int patched_rename_(const char *_Nonnull old_path, const char *_Nonnull new_path);

int patched_link_(const char *_Nonnull old_path, const char *_Nonnull new_path);
int patched_symlink_(const char *_Nonnull old_path, const char *_Nonnull new_path);
int patched_unlink_(const char *_Nonnull path);
ssize_t patched_readlink_(const char *_Nonnull path, char *_Nonnull buf, size_t buf_size);

int patched_stat_(const char *_Nonnull path, struct stat *_Nonnull buf);
int patched_lstat_(const char *_Nonnull path, struct stat *_Nonnull buf);

FILE *_Nullable patched_fopen_(const char *_Nonnull path, const char *_Nonnull mode);
int patched_fclose_(FILE *_Nonnull fp);
size_t patched_fread_(void *_Nonnull buf, size_t size, size_t count, FILE *_Nonnull fp);
long patched_ftell_(FILE *_Nonnull fp);
int patched_fseek_(FILE *_Nonnull fp, long offset, int whence);
int patched_fgetc_(FILE *_Nonnull fp);
int patched_ferror_(FILE *_Nonnull fp);
void patched_clearerr_(FILE *_Nonnull fp);
int patched_feof_(FILE *_Nonnull fp);
char *_Nullable patched_fgets_(char *_Nonnull buf, int size, FILE *_Nonnull fp);
int patched_fscanf_(FILE *_Nonnull fp, const char *_Nonnull fmt, ...) __scanflike(2, 3);
int patched_vfscanf_(FILE *_Nonnull fp, const char *_Nonnull fmt, va_list args) __scanflike(2, 0);

int patched_fflush_(MiniFILE *_Nullable fp);
size_t patched_fwrite_(const void *_Nonnull buf, size_t size, size_t count, FILE *_Nonnull fp);
int patched_fprintf_(FILE *_Nonnull fp, const char *_Nonnull fmt, ...) __printflike(2, 3);

//int patched_fcntl_(int fd, int op, ...);
int patched_fileno_(FILE *_Nonnull fp);

int patched_mkdir_(const char *_Nonnull path, mode_t mode);
int patched_rmdir_(const char *_Nonnull path);
DIR *_Nullable patched_opendir_(const char *_Nonnull path);
struct dirent *_Nullable patched_readdir_(DIR *_Nonnull dir);
int patched_closedir_(DIR *_Nonnull dir);

int patched_utime_(const char *_Nonnull filename, const struct utimbuf *_Nullable times);
int patched_setvbuf_(FILE *_Nonnull fp, char *_Nullable buf, int mode, size_t size);

#endif //NEEDLE_PATCHED_FUNCTIONS_H
