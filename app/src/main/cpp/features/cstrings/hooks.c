#include <string.h>
#include "cstrings.h"
#include "hooks.h"
#include "log.h"

#define LOG_TAG "MiniCStringHooks"

// For these offsets, basically find any place a basic_string is created, and look at the function.
// The function has error strings inside like "basic_string::_S_construct null not valid".

// For 64bit, it's called directly.

// For 32bit a small function is called, which calls a longer one. The error strings are in the longer function.
// However, we put the hook on the shorter function.

// 1.4.9 offsets: archSplit(0x37bbe0, 0x565b48)

#define CREATE_BASIC_STRING_OFFSET archSplit(0x37bc60, 0x566bb8)

DL_FUNCTION_OFFSET(
	create_basic_string,
	CREATE_BASIC_STRING_OFFSET,
	void, (CppString * *destinationPointer, const char *strDataPointer)
)

STATIC_DL_HOOK_OFFSET(
	create_basic_string,
	CREATE_BASIC_STRING_OFFSET,
	void, (CppString * *destinationPointer, const char *strDataPointer)
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

// To find the Append function, head to Caver::InventoryItemPanel::UpdateLabels, and find the call
// using the " Damage" string right before the GUILabel::AddText call.
// It will direct to a short function, which you should click through to the call that short function makes.
// The body will look kinda like the `create_basic_string` one.

STATIC_DL_HOOK_OFFSET(
	append_basic_string,
	archSplit(0x379988, 0x567254),
	void, (CppString * *destinationPointer, const char *strDataPointer, unsigned long length)
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

// To find the Assign function, head to Caver::ConfigureOverlayView::InitWithGameState, and find
// the call using the "Target Info" string. Click into this function.
// The body will look kinda like the `create_basic_string` one.

#define ASSIGN_BASIC_STRING_OFFSET archSplit(0x37aa1c, 0x56918c)
STATIC_DL_HOOK_OFFSET(
	assign_basic_string,
	ASSIGN_BASIC_STRING_OFFSET,
	void, (CppString * *destinationPointer, const char *strDataPointer, unsigned long length)
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

void init_feature_cstrings() {
	LOGD("Applying CString Replacement patch");

//	cstrings_add_replacement("Saved Games", "Hooks are working.");
//	cstrings_add_replacement("Attack", "Thwack!!");
//	cstrings_add_replacement("Health", "Your Life");

	hook_create_basic_string();
	hook_append_basic_string();
	hook_assign_basic_string();

	// Make create_basic_string available to the rest of Mini.
	// Using dlsym crashes 32bit for some reason, so use the original I guess...
	// The only affect this has is that mod-defined replacements won't apply.
	create_basic_string = orig_create_basic_string;
}
