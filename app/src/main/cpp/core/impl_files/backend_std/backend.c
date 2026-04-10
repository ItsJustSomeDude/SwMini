#include "backend.h"

#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

MiniFileBackend backend_std = {
	/* dirent.h */

	.opendir = std_opendir,
	.readdir = std_readdir,
	.closedir = std_closedir,

	/* stat.h */

	.stat = std_stat,
	.lstat = std_lstat,
	.mkdir = mkdir,

	/* stdio.h */

	.remove = remove,
	.rename = rename,

	.fopen = std_fopen,
	.fclose = std_fclose,
	.fread = std_fread,
	.ftell = std_ftell,
	.fseek = std_fseek,
	.fgetc = std_fgetc,
	.getc = std_getc,
	.ferror = std_ferror,
	.clearerr = std_clearerr,
	.feof = std_feof,
	.fgets = std_fgets,
	.fscanf = std_fscanf,
	.vfscanf = std_vfscanf,
	.fwrite = std_fwrite,
	.vfprintf = std_vfprintf,
	.fprintf = std_fprintf,
	.fflush = std_fflush,
	.fcntl = std_fcntl,
	.fileno = std_fileno,
	.setvbuf = std_setvbuf,

	/* stdlib.h */

	.mkstemp = std_mkstemp,

	/* unistd.h */

	.chdir = std_chdir,
	.getcwd = std_getcwd,
	.rmdir = rmdir,
	.link = link,
	.symlink = symlink,
	.unlink = unlink,
	.readlink = std_readlink,

	/* utime.h */

	.utime = std_utime,
};
