#ifndef NEEDLE_FILES_BACKEND_H
#define NEEDLE_FILES_BACKEND_H

#include <stdarg.h>
#include <sys/types.h>

typedef struct mini_dirent mini_dirent;
typedef struct mini_timespec mini_timespec;
typedef struct mini_utimbuf mini_utimbuf;
typedef struct mini_stat mini_stat;

typedef struct MiniFILE MiniFILE;
typedef struct MiniDIR MiniDIR;

#define IMPL_BACKEND_BODY_DIRENT_(pnt, pfx) \
    /* dirent.h */ \
    \
    MiniDIR *_Nullable (pnt pfx##opendir)(const char *_Nonnull path); \
    mini_dirent *_Nullable (pnt pfx##readdir)(MiniDIR *_Nonnull dir); \
    int (pnt pfx##closedir)(MiniDIR *_Nonnull dir)

#define IMPL_BACKEND_BODY_STAT_(pnt, pfx) \
    /* stat.h */ \
    \
    int (pnt pfx##stat)(const char *_Nonnull path, mini_stat *_Nonnull buf); \
    int (pnt pfx##lstat)(const char *_Nonnull path, mini_stat *_Nonnull buf); \
    \
    int (pnt pfx##mkdir)(const char *_Nonnull path, mode_t mode)

#define IMPL_BACKEND_BODY_STDIO_(pnt, pfx) \
    /* stdio.h */ \
    \
    int (pnt pfx##remove)(const char *_Nonnull path_string); \
    int (pnt pfx##rename)            \
        (const char *_Nonnull old_path_string, const char *_Nonnull new_path_string); \
    \
    MiniFILE *_Nullable (pnt pfx##fopen)                     \
        (const char *_Nonnull path_string, const char *_Nonnull mode); \
    int (pnt pfx##fclose)(MiniFILE *_Nonnull fp); \
    size_t (pnt pfx##fread)(void *_Nonnull buf, size_t size, size_t count, MiniFILE *_Nonnull fp); \
    __nodiscard long (pnt pfx##ftell)(MiniFILE *_Nonnull fp); \
    int (pnt pfx##fseek)(MiniFILE *_Nonnull fp, long offset, int whence); \
    __nodiscard int (pnt pfx##fgetc)(MiniFILE *_Nonnull fp); \
    __nodiscard int (pnt pfx##getc)(MiniFILE *_Nonnull fp); \
    \
    __nodiscard int (pnt pfx##ferror)(MiniFILE *_Nonnull fp); \
    void (pnt pfx##clearerr)(MiniFILE *_Nonnull fp); \
    __nodiscard int (pnt pfx##feof)(MiniFILE *_Nonnull fp); \
    char *_Nullable (pnt pfx##fgets)(char *_Nonnull buf, int size, MiniFILE *_Nonnull fp); \
    int (pnt pfx##fscanf)(MiniFILE *_Nonnull fp, const char *_Nonnull fmt, ...) __scanflike(2, 3); \
    int (pnt pfx##vfscanf) \
        (MiniFILE *_Nonnull fp, const char *_Nonnull fmt, va_list args) __scanflike(2, 0); \
    \
    size_t (pnt pfx##fwrite) \
        (const void *_Nonnull buf, size_t size, size_t count, MiniFILE *_Nonnull fp); \
    int (pnt pfx##vfprintf) \
        (MiniFILE *_Nonnull fp, const char *_Nonnull fmt, va_list va) __printflike(2, 0); \
    int (pnt pfx##fprintf)           \
        (MiniFILE *_Nonnull fp, const char *_Nonnull fmt, ...) __printflike(2, 0); \
    int (pnt pfx##fflush)(MiniFILE *_Nullable fp); \
    \
    int (pnt pfx##fcntl)(int fd, int op, ...); \
    int (pnt pfx##fileno)(MiniFILE *_Nonnull fp); \
    \
    int (pnt pfx##setvbuf)(MiniFILE *_Nonnull fp, char *_Nullable buf, int mode, size_t size)

#define IMPL_BACKEND_BODY_STDLIB_(pnt, pfx) \
    /* stdlib.h */ \
    \
    MiniFILE *_Nullable (pnt pfx##mkstemp)(char* _Nonnull path_template) \

#define IMPL_BACKEND_BODY_UNISTD_(pnt, pfx) \
    /* unistd.h */ \
    \
    int (pnt pfx##chdir)(const char *_Nonnull path); \
    char *_Nullable (pnt pfx##getcwd)(char *_Nullable buf, size_t size); \
    \
    int (pnt pfx##rmdir)(const char *_Nonnull path); \
    \
    int (pnt pfx##link)(const char *_Nonnull old_path, const char *_Nonnull new_path); \
    int (pnt pfx##symlink)(const char *_Nonnull old_path, const char *_Nonnull new_path); \
    int (pnt pfx##unlink)(const char *_Nonnull path); \
    ssize_t (pnt pfx##readlink)(const char *_Nonnull path, char *_Nonnull buf, size_t buf_size)

#define IMPL_BACKEND_BODY_UTIME_(pnt, pfx) \
    /* utime.h */ \
    \
    int (pnt pfx##utime)(const char *_Nonnull filename, const mini_utimbuf *_Nullable times)

typedef struct MiniFileBackend {
	IMPL_BACKEND_BODY_DIRENT_(*_Nullable,);
	IMPL_BACKEND_BODY_STAT_(*_Nullable,);
	IMPL_BACKEND_BODY_STDIO_(*_Nullable,);
	IMPL_BACKEND_BODY_STDLIB_(*_Nullable,);
	IMPL_BACKEND_BODY_UNISTD_(*_Nullable,);
	IMPL_BACKEND_BODY_UTIME_(*_Nullable,);
} MiniFileBackend;

#define FILE_BACKEND_HEADER(pfx) \
    IMPL_BACKEND_BODY_DIRENT_(,pfx); \
    IMPL_BACKEND_BODY_STAT_(,pfx); \
    IMPL_BACKEND_BODY_STDIO_(,pfx);  \
    IMPL_BACKEND_BODY_STDLIB_(,pfx); \
    IMPL_BACKEND_BODY_UNISTD_(,pfx); \
    IMPL_BACKEND_BODY_UTIME_(,pfx)

/* TODO: Maybe don't leak all those macros somehow? Idk. */

#endif /* NEEDLE_FILES_BACKEND_H */
