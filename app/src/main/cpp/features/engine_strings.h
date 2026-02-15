#ifndef NEEDLE_ENGINE_STRINGS_H
#define NEEDLE_ENGINE_STRINGS_H

/**
 * Convert a string from UTF8 (with up to 2 byte chars) to a byte sequence, in place.
 *
 * If `len == 0`, it will be auto-determined.
 * If new_len is a pointer, the new length will be output to it.
 */
void utf8_to_byte_sequence(char *string, size_t len, size_t *new_len);

#endif //NEEDLE_ENGINE_STRINGS_H
