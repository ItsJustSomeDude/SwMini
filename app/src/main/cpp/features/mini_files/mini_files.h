#ifndef NEEDLE_MINI_FILES_H
#define NEEDLE_MINI_FILES_H

#include <stddef.h>
#include <linux/limits.h>

#define EXT_RESOURCES_SUBDIR "/assets/"
#define APK_RESOURCES_SUBDIR "resources/"

#define PrefixFiles     "/Files"
#define PrefixCache     "/Cache"
#define PrefixExtFiles  "/ExternalFiles"
#define PrefixExtCache  "/ExternalCache"
#define PrefixAssets    "/Assets"

/* This is different, because it's baked into the engine. */
#define PrefixResources "resources"

typedef enum {
	ERROR = -1,
	UNKNOWN = 0,
	REAL,
	ASSET,
	RESOURCE,
} MiniPathType;

typedef struct {
	MiniPathType type;
	size_t path_length;
//	bool readable;
//	bool writable;
	char path[PATH_MAX];
} MiniPath;

typedef struct {
	/* Size of path in path block, not including terminator. */
	size_t path_len;
	/* Offset from the start of the path block this path lives at. */
	size_t path_offset;
} PathBlockEntry;

typedef enum {
	PROFILE_EXTERNAL = 0,
	PROFILE_INTERNAL,
	GLOBAL_EXTERNAL,
	GLOBAL_INTERNAL,
	RES_PATH_COUNT
} GlobalResourcePath;

/* The block of memory that holds all the asset paths. */
typedef struct {
	PathBlockEntry entries[RES_PATH_COUNT];

	/* The raw block of memory that has _all_ the paths, end-to-end, with terminators. */
	char path_block[];
} ResourcePathBlock;

/** The global block of resource paths, all in contiguous memory for <i>SPEED!</i> */
extern ResourcePathBlock *g_resource_paths;

/* This should be in the same order as `g_resource_paths` */
const char *mini_prefixes[4];

void set_profile_resource_path(const char *profile_id);
void set_global_resource_paths();

void parse_mini_path(MiniPath *_Nonnull out, const char *_Nullable input);

MiniPathType find_resource(char *_Nonnull out, size_t out_size, const char *_Nonnull input);

void real_to_mini_path(char *_Nonnull out, size_t out_size, const char *_Nonnull path);

#endif //NEEDLE_MINI_FILES_H
