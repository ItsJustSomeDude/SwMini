/* clang-format off */
#include "assets.h" /* clang-format on */

/* This file is an implementation for the Asset Hash Tables that will be used various places.
 * The headers for this are in "assets.h". */

__KHASH_IMPL(
	tbl, kh_inline klib_unused,
	kh_cstr_t, AssetEntry*,
	1, kh_str_hash_func, kh_str_hash_equal
)
