#include "core/hooks.h"
#include "offsets/cpp_strings.h"
#include "core/cpp_strings.h"
#include "init/features.h"

#include "cstrings.h"
#include "core/log.h"

#include <string.h>

#define LOG_TAG "MiniCStringHooks"

STATIC_DL_HOOK_OFFSET(
	create_basic_string,
	EOFF_CPP_STRING_FROM_CHAR_P,
	void, (CppString * destinationPointer, const char *strDataPointer)
) {
	if (strDataPointer == NULL || *strDataPointer == '\0') {
		orig_create_basic_string(destinationPointer, strDataPointer);
		return;
	}

	const char *replacement = cstrings_get_replacement(strDataPointer);
	if (replacement) {
		orig_create_basic_string(destinationPointer, replacement);
	} else {
		orig_create_basic_string(destinationPointer, strDataPointer);
	}
}

STATIC_DL_HOOK_OFFSET(
	append_basic_string,
	EOFF_CPP_STRING_APPEND_IMPL,
	void, (CppString * destinationPointer, const char *strDataPointer, unsigned long length)
) {
	if (strDataPointer == NULL || *strDataPointer == '\0') {
		orig_append_basic_string(destinationPointer, strDataPointer, length);
		return;
	}

	const char *replacement = cstrings_get_replacement(strDataPointer);
	if (replacement) {
		orig_append_basic_string(destinationPointer, replacement, strlen(replacement));
	} else {
		orig_append_basic_string(destinationPointer, strDataPointer, length);
	}
}

STATIC_DL_HOOK_OFFSET(
	assign_basic_string,
	EOFF_CPP_STRING_ASSIGN,
	void, (CppString * destinationPointer, const char *strDataPointer, unsigned long length)
) {
	if (strDataPointer == NULL || *strDataPointer == '\0') {
		orig_assign_basic_string(destinationPointer, strDataPointer, length);
		return;
	}

	const char *replacement = cstrings_get_replacement(strDataPointer);
	if (replacement) {
		orig_assign_basic_string(destinationPointer, replacement, strlen(replacement));
	} else {
		orig_assign_basic_string(destinationPointer, strDataPointer, length);
	}
}

void initF_cstrings() {
	LOGD("Applying CString Replacement patch");

//	cstrings_add_replacement("Saved Games", "Hooks are working.");
//	cstrings_add_replacement("Attack", "Thwack!!");
//	cstrings_add_replacement("Health", "Your Life");

	hook_create_basic_string();
	hook_append_basic_string();
	hook_assign_basic_string();

	miniCStr_read_asset("cstrings.toml");
}
