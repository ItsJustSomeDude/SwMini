#ifndef NEEDLE_PATCHED_STDIO_H
#define NEEDLE_PATCHED_STDIO_H

#include "core/files/stdio.h"

/* Replace built-in types with Mini types. */
#define FILE MiniFILE

/* stdio.h functions, patched to use Mini Files. */

#define remove(...) miniF_remove(__VA_ARGS__)
#define rename(...) miniF_rename(__VA_ARGS__)

#define fopen(...) miniF_fopen(__VA_ARGS__)
#define fclose(...) miniF_fclose(__VA_ARGS__)
#define fread(...) miniF_fread(__VA_ARGS__)
#define ftell(...) miniF_ftell(__VA_ARGS__)
#define fseek(...) miniF_fseek(__VA_ARGS__)
#define fgetc(...) miniF_fgetc(__VA_ARGS__)
#define getc(...) miniF_fgetc(__VA_ARGS__)
#define ferror(...) miniF_ferror(__VA_ARGS__)
#define clearerr(...) miniF_clearerr(__VA_ARGS__)
#define feof(...) miniF_feof(__VA_ARGS__)
#define fgets(...) miniF_fgets(__VA_ARGS__)
#define fscanf(...) miniF_fscanf(__VA_ARGS__)
#define fwrite(...) miniF_fwrite(__VA_ARGS__)
#define fprintf(...) miniF_fprintf(__VA_ARGS__)
#define fflush(...) miniF_fflush(__VA_ARGS__)

/* I can't really patch this. LuaFS uses it, but it should fail because `fileno` fails on Assets. */
// #define fcntl(...) miniF_fcntl(__VA_ARGS__)
#define fileno(...) miniF_fileno(__VA_ARGS__)

#define setvbuf(...) miniF_setvbuf(__VA_ARGS__)

#endif /* NEEDLE_PATCHED_STDIO_H */
