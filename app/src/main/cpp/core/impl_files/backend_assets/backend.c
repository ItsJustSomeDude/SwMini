#include "backend.h"

MiniFileBackend backend_assets = {
	/* dirent.h */

	.opendir = assets_opendir,
	.readdir = assets_readdir,
	.closedir = assets_closedir,

	/* stat.h */

	.stat = assets_stat,
	.lstat = assets_lstat,
	.mkdir = assets_mkdir,

	/* stdio.h */

	.remove = assets_remove,
	.rename = assets_rename,

	.fopen = assets_fopen,
	.fclose = assets_fclose,
	.fread = assets_fread,
	.ftell = assets_ftell,
	.fseek = assets_fseek,
	.fgetc = assets_fgetc,
	.getc = assets_getc,
	.ferror = assets_ferror,
	.clearerr = assets_clearerr,
	.feof = assets_feof,
	.fgets = assets_fgets,
	.fscanf = assets_fscanf,
	.vfscanf = assets_vfscanf,
	.fwrite = assets_fwrite,
	.vfprintf = assets_vfprintf,
	.fprintf = assets_fprintf,
	.fflush = assets_fflush,
	.fcntl = assets_fcntl,
	.fileno = assets_fileno,
	.setvbuf = assets_setvbuf,

	/* stdlib.h */

	.mkstemp = assets_mkstemp,

	/* unistd.h */

	.chdir = assets_chdir,
	.getcwd = assets_getcwd,
	.rmdir = assets_rmdir,
	.link = assets_link,
	.symlink = assets_symlink,
	.unlink = assets_unlink,
	.readlink = assets_readlink,

	/* utime.h */

	.utime = assets_utime,
};
