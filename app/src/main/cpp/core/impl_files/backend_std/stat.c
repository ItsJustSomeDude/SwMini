#include "backend.h"

#include "core/files/stat.h"
#include "core/impl_files/mini_file.h"

#include <sys/stat.h>
#include <string.h>

static void copy_stat(struct stat *from, mini_stat *to) {
	from->st_dev = to->st_dev;
	from->st_ino = to->st_ino;
	from->st_mode = to->st_mode;
	from->st_nlink = to->st_nlink;
	from->st_uid = to->st_uid;
	from->st_gid = to->st_gid;
	from->st_rdev = to->st_rdev;
	from->st_size = to->st_size;
	from->st_blksize = to->st_blksize;
	from->st_blocks = to->st_blocks;

	from->st_atim.tv_nsec = to->st_atim.tv_nsec;
	from->st_atim.tv_sec = to->st_atim.tv_sec;

	from->st_mtim.tv_nsec = to->st_mtim.tv_nsec;
	from->st_mtim.tv_sec = to->st_mtim.tv_sec;

	from->st_ctim.tv_nsec = to->st_ctim.tv_nsec;
	from->st_ctim.tv_sec = to->st_ctim.tv_sec;
}

int std_stat(const char *path_string, mini_stat *_Nonnull buf) {
	memset(buf, 0, sizeof(*buf));

	struct stat s;
	int ret = stat(path_string, &s);
	copy_stat(&s, buf);

	return ret;
}

int std_lstat(const char *path_string, mini_stat *_Nonnull buf) {
	memset(buf, 0, sizeof(*buf));

	struct stat s;
	int ret = lstat(path_string, &s);
	copy_stat(&s, buf);

	return ret;
}
