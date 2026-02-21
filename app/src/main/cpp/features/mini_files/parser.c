
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "mini_files.h"
#include "global.h"
#include "log.h"

#define LOG_TAG "MiniPathParser"

/**
 * Given a pointer to a segment in the buffer, amount of space left in buffer,
 * the length of the segment, either leave the segment where it is, remove it, or replace it based
 * on the content. Since it reads and writes from the same place, it cannot affect anything before
 * the current. That's ok though, because Path Traversal is blocked by this too.
 * <br><br>
 * It can also output the `type` if it is influenced by this segment.
 * <br><br>
 * The segment should include the leading '/', but not the trailing.
 *
 * Returns the new position for writing the next segment, or -1 on failure.
 */
size_t mini_path_append(
	char *segment_buffer, size_t remaining_buffer_size,
	size_t segment_length, size_t segment_offset,
	MiniPathType *type
) {
//	LOGD(
//		"Processing segment '%s' (len %li). "
//		"This segment is at absolute position %li, and there are %li bytes of space.",
//		segment_buffer, segment_length,
//		segment_offset, len_left_in_buffer
//	);

	bool is_first_segment = segment_offset == 0;
	/** Segment with optional leading / removed. */
	char *without_lead;

	const char *replacement = NULL;

	if (is_first_segment && strcmp(segment_buffer, PrefixResources) == 0) {
		/* By far the most common path, because all assets flow through here. Remove the first
		 * segment, and on the next segment remove the leading '/'. */
		if (type != NULL) *type = RESOURCE;
		*segment_buffer = '\0';
		return 0; /* segment_offset is always 0 */

	} else if (is_first_segment && type != NULL && (*type == ASSET || *type == RESOURCE)) {
		/* The previous segment marked us as a Resource or an Asset, both of which should have the
		 * leading '/' stripped off.
		 * We're still in the first segment, so we need to shift the path left one byte to remove
		 * the leading '/'.
		 * MemMove the string, and shift the write pointer back one byte. */
		if (*(segment_buffer + 1) == '\0')
			/* There's only 1 char in the string, so just terminate it. */
			*segment_buffer = '\0';
		else
			/* memmove length+1 to include the NULL terminator. */
			memmove(segment_buffer, segment_buffer + 1, segment_length + 1);
		return segment_length - 1; /* segment_offset is always 0 */

	} else if (is_first_segment && strcmp(segment_buffer, PrefixAssets) == 0) {
		/* For '/Assets', we remove this path segment, and also need to remove the '/' from
		 * the *next* segment. */

		if (type != NULL) *type = ASSET;
		*segment_buffer = '\0';
		return 0; /* segment_offset is always 0 */

	} else if (is_first_segment && strcmp(segment_buffer, PrefixFiles) == 0) {
		replacement = g_files_path;
	} else if (is_first_segment && strcmp(segment_buffer, PrefixExtFiles) == 0) {
		replacement = g_ext_files;
	} else if (is_first_segment && strcmp(segment_buffer, PrefixCache) == 0) {
		replacement = g_cache_path;
	} else if (is_first_segment && strcmp(segment_buffer, PrefixExtCache) == 0) {
		replacement = g_ext_cache;
	} else if (
		strcmp(segment_buffer, "/") == 0 ||
		(without_lead = *segment_buffer == '/' ? segment_buffer + 1 : segment_buffer,
			strcmp(without_lead, ".") == 0)
		) {
		/* Useless path segment, remove it. Set the write pointer to the current position. */
		*segment_buffer = '\0';
		return segment_offset;

	} else if (is_first_segment || strcmp(without_lead, "..") == 0) {
		/* First segment did not match any allowed paths, or attempted Path Traversal. */
		*segment_buffer = '\0';
		if (type != NULL) *type = ERROR;
		return -1;
	}
	/* else, it's just a regular path segment, so just fall through. */

	if (replacement != NULL) {
		size_t len = strlen(replacement);
		if (len > remaining_buffer_size) {
			LOGE("Path too long! '%s'", replacement);
			/* Terminate buffer, set Error flag, return -1. */
			*segment_buffer = '\0';
			if (type != NULL) *type = ERROR;
			return -1;
		}
		strcpy(segment_buffer, replacement);
		return segment_offset + len;
	}

	/* No replacement. Leave segment as-is, advance pointer to end of it. */
	return segment_offset + segment_length;
}

/* Used for a debug line. */
// static const char *spaces[PATH_MAX] = {[0 ... PATH_MAX - 2] = (const char *) ' '};

/**
 * Read a Mini path, and convert it to a real FS path, or a relative Asset path in case of an
 * asset. Returns a heap allocated structure with the Type, New Path, and New Path Length.
 * <br><br>
 * In case of invalid Mini path, NULL is returned.
 */
void parse_mini_path(MiniPath *out, const char *_Nullable input) {
	/* Instant fail if input is NULL or empty string */
	if (input == NULL || *input == '\0') {
		goto error;
	}

#define buffer_size (sizeof(out->path))
#define buffer      (out->path)

	/* Position we are writing to `buffer`. This is the length at the end.
	 * It's always going to be one char *after* what we've written. */
	size_t write_pos = 0;
	/* Position we are reading from `input` */
	size_t read_pos = 0;

	/** Position in the <b>Output</b> where the current path segment starts. This includes the '/' */
	size_t cur_seg_start = 0;
	/** Length of current path segment being read, including `/`. */
	size_t cur_seg_length = 0;

	size_t input_len = strlen(input);

	do {
		char cur_char = input[read_pos];
//		LOGD("Reading byte %li, char is '%c', seg is %li",
//		     read_pos, cur_char == '\0' ? '0' : cur_char,
//		     cur_seg_length
//		);

		if (read_pos > 0 && (cur_char == '/' || cur_char == '\0')) {
			/* We have read at least one char, and we hit a separator or end of string.
			 * The current segment should be... */

			buffer[write_pos] = '\0';
			char *current_segment = buffer + cur_seg_start;
//			LOGD("Completed Segment: '%s' with len %li", current_segment, cur_seg_length);
//			LOGD("%s", buffer);
//			LOGD("%s^", spaces + strlen(spaces) - cur_seg_start);

			write_pos = mini_path_append(
				current_segment,
				buffer_size - cur_seg_start - 1,
				cur_seg_length,
				cur_seg_start,
				&out->type
			);
			if (write_pos == -1) {
				goto error;
			}

			/* Current segment starts at *this* position.
			 * The length starts at 1, because we're already on the / char that will be the first char of the next segment. */
			cur_seg_start = write_pos;
			cur_seg_length = 1;
			buffer[write_pos++] = cur_char;
		} else {
			/* Regular Char. Write it to the output, advance write pointer. */
			buffer[write_pos++] = cur_char;
			cur_seg_length++;
		}
	} while (read_pos++ < input_len);

	LOGD("Converted Path: '%s' -> '%s' (%li)", input, buffer, write_pos - 1);

	out->path_length = write_pos - 1;
//	if (out->type == ERROR || out->type == UNKNOWN) {
//		out->readable = false;
//		out->writable = false;
//	} else if (out->type == RESOURCE || out->type == ASSET) {
//		out->readable = true;
//		out->writable = false;
//	} else if (out->type == REAL) {
//		out->readable = true;
//		out->writable = true;
//	}
	return;

	error:
	LOGW("Invalid MiniPath: '%s'", input);
	out->type = ERROR;
//	out->readable = out->writable = false;
	out->path_length = 0;
	*out->path = '\0';
}

void real_to_mini_path(char *out, size_t out_size, const char *path) {
	const size_t path_count = sizeof(g_path_list) / sizeof(g_path_list[0]);
	for (size_t i = 0; i < path_count; i++) {
		if (g_path_list[i] == NULL || *g_path_list[i] == NULL)
			continue; /* No entry in this slot. */

		/* The current Global path we are checking. */
		const char *cur_path = *g_path_list[i];
		size_t cur_path_len = strlen(cur_path);

		LOGD("Checking '%s' & '%s'", cur_path, path);

		/* Check if the entirety of the global path is present at the start of the input path. */
		if (strncasecmp(path, cur_path, cur_path_len) == 0) {
			/* Extract the suffix from the input path (remove the global part). */
			const char *path_suffix = path + cur_path_len;
			size_t suffix_len = strlen(path_suffix);

			const char *new_prefix = mini_prefixes[i];
			size_t prefix_len = strlen(new_prefix);

			if (prefix_len + suffix_len > out_size - 1) {
				LOGW("Length exceeded while encoding path '%s'", path);
				*out = '\0';
				return;
			}

			memcpy(out, new_prefix, prefix_len);
			memcpy(out + prefix_len, path_suffix, suffix_len + 1);

			LOGD("Converted real path to MiniPath: '%s' -> '%s'", path, out);
			return;
		}
	}
}
