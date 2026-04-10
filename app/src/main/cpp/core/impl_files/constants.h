#ifndef NEEDLE_FILES_IMPL_CONSTANTS_H
#define NEEDLE_FILES_IMPL_CONSTANTS_H

/* The maximum of all path strings. */
#define MINI_PATH_SIZE 1024

/* This is the subdirectory for Resources to be found in the external directories, like
 * /sdcard/Android/files/<pkg>/SUBDIR */
#define EXT_RESOURCES_SUBDIR "/resources/"

/* This is the subdir path in the assets folder in the APK. */
#define APK_RESOURCES_SUBDIR "resources/"

/* These are the user-facing Prefixes that will be used at the start of Mini Paths. */
#define PrefixFiles     "/Files"
#define PrefixCache     "/Cache"
#define PrefixExtFiles  "/ExternalFiles"
#define PrefixExtCache  "/ExternalCache"
#define PrefixAssets    "/Assets"

/* This is like the above Prefixes, but must be what's baked into the engine. */
#define PrefixResources "resources"

#endif /* NEEDLE_FILES_IMPL_CONSTANTS_H */
