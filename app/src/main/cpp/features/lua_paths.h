#ifndef NEEDLE_LUA_PATHS_H
#define NEEDLE_LUA_PATHS_H

#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utime.h>

struct stat;
struct utimbuf;

#define EXTERNAL_FILES_BASE "/ExternalFiles/"
#define FILES_BASE "/Files/"
#define EXTERNAL_CACHE_BASE "/ExternalCache/"
#define CACHE_BASE "/Cache/"

const char *from_mini_path(char *out, size_t out_size, const char *path);
const char *to_mini_path(char *out, size_t out_size, const char *path);

// Here are the functions that take MiniPaths.

int mini_path_chdir(const char *_Nonnull path);
char *_Nullable mini_path_getcwd(char *_Nullable buf, size_t size);

int mini_path_symlink(const char *_Nonnull old_path, const char *_Nonnull new_path);
int mini_path_link(const char *_Nonnull old_path, const char *_Nonnull new_path);
int mini_path_unlink(const char *_Nonnull path);
ssize_t mini_path_readlink(const char *_Nonnull path, char *_Nonnull buf, size_t buf_size);

int mini_path_mkdir(const char *_Nonnull path, mode_t mode);
int mini_path_rmdir(const char *_Nonnull path);

int mini_path_utime(const char *_Nonnull filename, const struct utimbuf *_Nullable times);
//int mini_path_fileno(FILE *_Nonnull fp);
//int mini_path_fcntl(int fd, int op, ...);

FILE *_Nullable mini_path_fopen(const char *_Nonnull path, const char *_Nonnull mode);
//int mini_path_fclose(FILE *_Nonnull fp);

DIR *_Nullable mini_path_opendir(const char *_Nonnull path);
struct dirent *_Nullable mini_path_readdir(DIR *_Nonnull dir);
//int mini_path_closedir(DIR *_Nonnull dir);

int mini_path_remove(const char *_Nonnull path);
int mini_path_rename(const char *_Nonnull old_path, const char *_Nonnull new_path);

int mini_path_lstat(const char *_Nonnull path, struct stat *_Nonnull buf);
int mini_path_stat(const char *_Nonnull path, struct stat *_Nonnull buf);

// TODO: Must be hooked: Lua's loadFile
// doFile is a macro so doesn't need to change.

#endif //NEEDLE_LUA_PATHS_H
