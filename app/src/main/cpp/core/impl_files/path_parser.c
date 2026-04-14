#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "path_parser.h"
#include "constants.h"
#include "core/log.h"
#include "core/impl_files/backend_assets/backend.h"
#include "paths.h"
#include "core/impl_files/backend_std/backend.h"
#include "resource_paths.h"

#define LOG_TAG "MiniProcessPath"

/* string left compare: Checks if string1 starts with string2.
 * Does double evaluation. */
#define strlcmp(lhs, rhs) strncasecmp(lhs, rhs, strlen(rhs))

/* Predeclare... */
static void replace_first_segment(
	char *out, size_t out_size, const char *path, const char *replacement
);

/* This is the main function for the Files system. It takes a MiniPath, and returns a Parsed
 * MiniPath with the Backend and the translated path.
 *
 * Here are the rules for parsing paths:
 * "resources/" -> Resource Search Path Hierarchy, backend either Assets or Standard
 *
 * "/ExternalFiles/", etc.s -> Replace first segment with base path, Standard Backend
 * ""
 */
void miniP_parse(ParsedPath *out, const char *raw_path) {
	char path[MINI_PATH_SIZE] = {0};
	miniP_simplify(path, raw_path);

	if (strlcmp(path, "resources/") == 0) {
		/* Remove first segment, then locate resource based on rules (ExtAss, then APK.) */
		const char *res_name = path + strlen("resources/");

		/* TODO: Attempt to locate actually... */
		miniR_locate_resource(out, res_name);

	} else if (g_ext_files != NULL && strlcmp(path, "/ExternalFiles/") == 0) {
		replace_first_segment(out->path, sizeof(out->path), path, g_ext_files);
		out->backend = &backend_std;

	} else if (g_files_path != NULL && strlcmp(path, "/Files/") == 0) {
		replace_first_segment(out->path, sizeof(out->path), path, g_files_path);
		out->backend = &backend_std;

	} else if (g_ext_cache != NULL && strlcmp(path, "/ExternalCache/") == 0) {
		replace_first_segment(out->path, sizeof(out->path), path, g_ext_cache);
		out->backend = &backend_std;

	} else if (g_cache_path != NULL && strlcmp(path, "/Cache/") == 0) {
		replace_first_segment(out->path, sizeof(out->path), path, g_cache_path);
		out->backend = &backend_std;

	} else if (strlcmp(path, "/Assets/") == 0) {
		/* For "/Assets/", slice off the prefix */
		const size_t prefix_len = strlen("/Assets/");
		memmove(out->path, path + prefix_len, strlen(path) - prefix_len + 1);
		out->backend = &backend_assets;

	} else {
		LOGW("Attempted to parse invalid MiniPath: '%s'", path);
		out->backend = NULL;
		out->path[0] = '\0';
	}
}

static void replace_first_segment(
	char *out, size_t out_size, const char *path, const char *replacement
) {
	size_t replacement_len = strlen(replacement);

	bool is_absolute = (*path == '/');
	const char *after_first_segment = strchr(is_absolute ? path + 1 : path, '/');

	memcpy(out, replacement, replacement_len);
	strncpy(out + replacement_len, after_first_segment, out_size - replacement_len);
}

/*
 * Paths are valid if they are strings that do not contain path traversal. (/../)
 * Returns whether the path is valid. If invalid, out will be set to an empty string.
 *
 * Simplifying will remove any useless segments "//" and "/./"
 *
 * Since this will only ever shorten the path, we just need to check if the input is too long.
 *
 * I hate this function! Path traversal is so hard to get right, I'm just removing it.
 * Maybe I'll try again in the future... */
bool miniP_simplify(char *_Nonnull out, const char *_Nonnull path) {
	size_t read_pos = 0, write_pos = 0;
	size_t segment_offset = 0;

	size_t path_len = strlen(path);
	if (path_len > MINI_PATH_SIZE) {
		/* Path length exceeded. */
		out[0] = '\0';
		return false;
	}

	LOGD("\n\nStarting path: %s", path);

	/* "/hello/world/yeah.txt"
	 * ["/", "hello/", "world/", "yeah.txt"]
	 * "hello/world"
	 * ["hello/", "world"]
	 * "///hello///world"
	 * ["/", "/", "/", "hello/", "/", "/", "world"] */

	do {
		char cur_char = path[read_pos];
		out[write_pos] = cur_char;

		if (cur_char == '\0' || cur_char == '/') {
//			LOGD("Separator or End hit.");

			/* Possible cases. The Write pointer is on the last char, _ means NULL.
			 * string/    -> write
			 * string_    -> write
			 * ./         ->
			 * ._
			 * ../
			 * .._
			 * /          -> Useless, except when the first char of output. Back up by 1.
			 *               Can just use dereference, because there's no other cases that can start with '/'
			 * _          -> Can only happen at end of input, when input ends with a /.
			 */

			if (cur_char != '\0')
				/* Terminate string so we can use the segment. */
				out[write_pos + 1] = '\0';

			const char *segment = out + segment_offset;
			/* Options: ["", ".", "..", "whatever"] */
//			LOGD("Completed Segment: '%s', whole: '%s', write pos: %li, read pos: %li", segment,
//			     out, write_pos, read_pos);

			if (write_pos != 0 && *segment == '/') {
				write_pos--;
			} else if (strcmp(segment, "./") == 0) {
				write_pos -= 2;
			} else if (strcmp(segment, "../") == 0 || strcmp(segment, "..") == 0) {
				out[0] = '\0';
				return false;

//				if (write_pos < 4) write_pos = 0;
//				else write_pos -= 4;
//
//				while (write_pos > 0 && out[write_pos] != '/')
//					write_pos--;

//				do {
//					/* Buffer: "Test/Test/../"
//					 * Write:               ^
//					 * Want:        ^
//					 * First, back it up to the start of the current segment.
//					 * Then, back it up to the next '/' or start of string. */
//
//				} while (write_pos != 0 && );
//				/* We're sitting on the last '/', so we need to advance 1 to write after it. */
//				write_pos++;
			}

//			out[write_pos + 1] = '\0'; /* Temp */
//			LOGD("After stuff: whole: '%s', write pos: %li", out, write_pos);

			segment_offset = write_pos + 1;
		}

		write_pos++;
	} while (read_pos++ < path_len);

//	size_t len = strlen(out);
//	if (len > 1 && out[len - 1] == '/')
//		out[len - 1] = '\0';

	if (write_pos > 2 && out[write_pos - 2] == '/')
		out[write_pos - 2] = '\0';

	// LOGD("Final: '%s'", out);

	return true;
}

//void test_miniP_simplify() {
//	char out[4096];
//	char *in;
//#define _test(input, output) \
//    miniP_simplify(out, input); \
//    ((void)0)
//
////#define _test(input, output) \
////    miniP_simplify(out, input); \
////    LOGD("'%s' -> '%s'", input, out); \
////    if(strcmp(out, output) == 0) LOGD("Result: pass"); \
////    else LOGD("Result: !!!!! FAIL! Expected '%s'", output); \
////    ((void)0)
//
//	_test("", "");
//
//	_test("/", "/");
//	_test("/////", "/");
//
//	_test("/./", "/");
//	_test("/./././", "/");
//	_test("./", "");
//
//	_test("/a/b/c", "/a/b/c");
//	_test("/a//b///c/", "/a/b/c");
//
//	_test("/a/./b/./c/", "/a/b/c");
//	_test("./a/./b/", "a/b");
////
////	_test("/a/b/../c", "/a/c");
////	_test("/a/b/c/..", "/a/b");
////	_test("/a/b/c/../..", "/a");
////	_test("/a/b/c/../../..", "/");
////
////	_test("/a/b/c/../../../..", "/");
////
////	_test("/../", "/");
////	_test("/../../../../", "/");
//
////	_test("../", "");
////	_test("../../a", "a");
////
////	_test("/a/.././../b/", "/b");
////	_test("/a//../b/./c//", "/b/c");
////
////	_test("//a///b/../c/.", "/a/c");
////
////	_test("./../a/b", "a/b");
////	_test("./a/b/../../c/", "c");
////
//	_test("a/b/c", "a/b/c");
////	_test("a//b/./c/../d", "a/b/d");
////
////	_test("../a/../b/../c", "c");
////
////	_test("/a/../../b/../c//.//", "/c");
//}

/* This static array is the strings that lead to each index in the global_res_paths block. */
static const char *MiniPathPrefixes[4] = {
	PrefixExtFiles,
	PrefixFiles,
	PrefixExtCache,
	PrefixCache,
};

typedef struct thing {
	const char *real_prefix;
	const char *mini_prefix;
} thing;

void miniP_from_real(char *out, size_t out_size, const char *real_path) {
	/* Loop through all the set path strings (not resource paths...)
	 * If any is the prefix of `real_path`, replace it. */

	const thing things[] = {
		{.real_prefix = g_ext_files, .mini_prefix = PrefixExtFiles},
		{.real_prefix = g_files_path, .mini_prefix = PrefixFiles},
		{.real_prefix = g_ext_cache, .mini_prefix = PrefixExtCache},
		{.real_prefix = g_cache_path, .mini_prefix = PrefixCache},
	};

#define thing_count (sizeof(things)/sizeof(things[0]))
	/* const size_t test = thing_count; // -> 4 */

	for (int i = 0; i < thing_count; i++) {
		if (things[i].real_prefix == NULL) continue;

		const char *real_prefix = things[i].real_prefix;
		size_t prefix_len = strlen(real_prefix);

		if (strncasecmp(real_path, real_prefix, prefix_len) == 0) {
			const char *mini_prefix = things[i].mini_prefix;
			size_t mini_prefix_len = strlen(mini_prefix);

			const char *suffix = real_path + prefix_len;
			size_t suffix_len = strlen(real_path) - prefix_len;

			if (mini_prefix_len + suffix_len > out_size - 1) {
				LOGW("Length exceeded while encoding path '%s'", real_path);
				*out = '\0';
				return;
			}

			memcpy(out, mini_prefix, prefix_len);
			memcpy(out + mini_prefix_len, suffix, suffix_len + 1);

			LOGD("Converted real path to MiniPath: '%s' -> '%s'", real_path, out);
			return;
		}
	}

	/* I don't know what to do if this falls through? */
	LOGW("Invalid path while converting '%s'", real_path);

//	/* Loop through the global ResPath block, checking if the provided path has any as a prefix.
//	 * If it does, it gets the corresponding Mini-prefix, and replaces the real-path prefix. */
//
//	for (int i = 0; i < RES_PATH_COUNT; i++) {
//		ResPathEntry *header = &global_res_paths->entries[i];
//		if (header->path_pointer == NULL)
//			continue; /* No entry in this slot. */
//
//		const char *cur_path = header->path_pointer;
//		size_t cur_path_len = header->path_len;
//
//		LOGD("Checking '%s' & '%s'", cur_path, real_path);
//
//		/* Check if the entirety of the global path is present at the start of the input path. */
//		if (strncasecmp(real_path, cur_path, cur_path_len) == 0) {
//			/* Extract the suffix from the input path (remove the global part). */
//			const char *path_suffix = real_path + cur_path_len;
//			size_t suffix_len = strlen(path_suffix);
//
//			const char *new_prefix = MiniPathPrefixes[i];
//			size_t prefix_len = strlen(new_prefix);
//
//			if (prefix_len + suffix_len > out_size - 1) {
//				LOGW("Length exceeded while encoding path '%s'", real_path);
//				*out = '\0';
//				return;
//			}
//
//			memcpy(out, new_prefix, prefix_len);
//			memcpy(out + prefix_len, path_suffix, suffix_len + 1);
//
//			LOGD("Converted real path to MiniPath: '%s' -> '%s'", real_path, out);
//			return;
//		}
//	}

}
