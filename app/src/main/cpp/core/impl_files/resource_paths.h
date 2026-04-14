#ifndef NEEDLE_RESOURCE_PATHS_H
#define NEEDLE_RESOURCE_PATHS_H

#include <stddef.h>
#include "constants.h"
#include "path_parser.h"

/* Resource Path Block
 *
 * For CPU Cache optimization, Mini keeps a singleton structure containing all the filesystem paths
 * that will be tried for Resource location. When locating a resource, it will try each path, using
 *
 * In the structure in RAM, first are the alternating { offset, path } structures, then a single
 * block with all the strings end-to-end.
 */

/* This enum defines names for each index in the global ResPath block (`global_res_paths`) */
typedef enum ResPathIDs {
	PROFILE_EXTERNAL = 0,
	PROFILE_INTERNAL,
	STANDARD_EXTERNAL,
	STANDARD_INTERNAL,
	RES_PATH_COUNT
} ResPathIDs;

typedef struct ResPathEntry {
	/* A pointer to this entry's string. It will be a pointer into the global path block, but it's
	 * a valid string pointer nonetheless. */
	const char *_Nullable path_pointer;
	/* Same as strlen(path_pointer). (Size of path in path block, not including terminator.) */
	size_t path_len;
} ResPathEntry;

/* The block of memory that holds all the asset paths. */
typedef struct ResPaths {
	/* Info about the resource search paths for iteration. (size and pointer) */
	ResPathEntry entries[RES_PATH_COUNT];

	/* The raw block of memory that has _all_ the paths, end-to-end, with terminators. */
	char path_block[];
} ResPaths;

/** The global block of resource paths, all in contiguous memory for <i>SPEED!</i> */
extern ResPaths *_Nonnull global_res_paths;

void miniR_refresh_paths(void);

void miniR_locate_resource(ParsedPath *_Nonnull out, const char *_Nonnull path);

#endif /* NEEDLE_RESOURCE_PATHS_H */
