#ifndef NEEDLE_CPP_STRING_OFFSETS_H
#define NEEDLE_CPP_STRING_OFFSETS_H

#include "base.h"

/* Used everywhere across the engine. *Component::className calls it as the only action.
 * It's a short stub-like function on 32bit, and a full implementation on 64bit. */
#define EOFF_CPP_STRING_FROM_CHAR_P offset_(0x37bc60, 0x566bb8)

/* google::protobuf::MessageLite::ParseFromArray calls append three times in a row,
 * "Can't " + "parse" + " message of type \"". IMPL is called by that short function. */
#define EOFF_CPP_STRING_APPEND_IMPL offset_(0x379988, 0x567254)

/* Caver::ProgramTable::StringAtIndex calls Assign directly after the strlen call. */
#define EOFF_CPP_STRING_ASSIGN offset_(0x37aa1c, 0x56918c)

/* This function releases a CppString, without checking if it's the empty string sentinel first.
 * Find the string "basic_string::substr". Every usage of this string is a call to a function I
 * named "throw_formatted_exception", which takes a format string and varargs.
 * UnsafeRelease is called at the end of its body, inside a sentinel check. */
#define EOFF_CPP_STRING_UNSAFE_RELEASE offset_(0x3787c8, 0x565220)

#if defined(__arm__)
/* This function releases a CppString, if it's not the empty string sentinel.
 * It does not exist on 64bit due to being inlined!
 * On 32bit, it can be found anywhere strings are created and then deleted.
 * AchievementsManager::AchievementsManager calls it like 100 times in a row. */
#define EOFF_CPP_STRING_SAFE_RELEASE (0x379768)
#endif

/* This is used in constructors and String Deletion on 64bit.
 * Caver::Quest::Quest references it four times.
 * (Go back a few bytes from the empty data to the start of the block.) */
#define BOFF_CPP_STRING_EMPTY_SENTINEL offset_(0x6a04, 0x14880)

#endif //NEEDLE_CPP_STRING_OFFSETS_H
