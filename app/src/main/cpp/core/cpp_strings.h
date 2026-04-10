#ifndef NEEDLE_CPP_STRINGS_H
#define NEEDLE_CPP_STRINGS_H

#include <stddef.h>
#include <stdint.h>
#include "offsets/cpp_strings.h"

typedef struct CppString_Header {
	size_t length;
	size_t capacity;
	int uc;

	/* Padding is implicitly added on 64 bit, is already correct as-is for 32 bit. */
	/* int padding_; */

	/* The actual string data. */
	char string[];
} CppString_Header;

typedef char CppString;

/* Used for validation, should be 0x18 on 64bit, */
/* const size_t CppString_Header_Size = sizeof(CppString_Header); */

/* Shifts the pointer for an upside-down CPP String Struct, to allow accessing fields like length. */
#define ADJ(string) ((CppString_Header*) (string - sizeof(CppString_Header)))

/* Create a C++ String. */
extern void (*CppString_create)(CppString **out, const char *data);

/* Decrement the usage count of a string, deleting it if count hits 0. */
extern void CppString_release(CppString *string);

/* Release a string by its *base pointer*, even if it's an empty string sentinel. */
//extern void (*CppString_unsafe_release)(CppString_Header *string);

/* The static "Empty String" sentinel, embedded in the engine. */
extern CppString_Header *CppString_s_empty;

#endif //NEEDLE_CPP_STRINGS_H
