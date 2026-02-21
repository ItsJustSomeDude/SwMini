//#ifndef NEEDLE_USE_LUA_PATHS_H
//#define NEEDLE_USE_LUA_PATHS_H

#include "patched_functions.h"

/* This file defines Macros that overwrite the regular C functions with patched ones from Mini. */

#define chdir mini_path_chdir
#define getcwd mini_path_getcwd
#define symlink mini_path_symlink
#define link mini_path_link
#define unlink mini_path_unlink
#define mkdir mini_path_mkdir
#define rmdir mini_path_rmdir
#define opendir mini_path_opendir
#define utime mini_path_utime
#define readlink mini_path_readlink
#define fopen mini_path_fopen
//#define fclose mini_path_fclose
#define readdir mini_path_readdir
//#define closedir mini_path_closedir
//#define fcntl mini_path_fcntl
//#define fileno mini_path_fileno

#define remove mini_path_remove
#define rename mini_path_rename

#define lstat mini_path_lstat

// This macro doesn't work, because it steps on the Struct named `stat`.
//#define stat mini_path_stat

//#endif //NEEDLE_USE_LUA_PATHS_H
