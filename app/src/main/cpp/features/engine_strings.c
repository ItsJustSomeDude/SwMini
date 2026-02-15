#include <string.h>
#include "engine_strings.h"
#include "log.h"

#define LOG_TAG "MiniES"

void utf8_to_byte_sequence(char *_Nonnull string, size_t len, size_t *_Nullable new_len) {
	if (len == 0)
		len = strlen(string);

	int w = 0;
	for (int r = 0; r < len; r++) {
		if (string[r] <= 0x7F) {
			// ASCII byte, pass it through.
			string[w] = string[r];
		} else if (
			(string[r] & 0xE0) == 0xC0 &&
			(r + 1) < len
			) {
			// This is a surrogate byte. Use this and the next byte to convert to single codepoint,
			// place it at `w`, and advance the read by an extra char.
			string[w] = ((string[r] & 0x1F) << 6) | (string[r + 1] & 0x3F);
			r++;
		} else {
			// It's a 3+ byte char, or cut off surrogate. Mark it as a problem.
			string[w] = '?';
		}
		w++;
	}
	// Re-terminate the potentially shortened string.
	string[w] = '\0';

	if (new_len != NULL)
		*new_len = w;
}
