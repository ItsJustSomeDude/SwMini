/* clang-format off */
#include "cstrings.h" /* clang-format on */

#include "assembly/arm64.h"
#include "hooks.h"
#include "log.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define LOG_TAG "MiniCStringWorkarounds"

void setup_ui_attack_patch() {

#ifdef __arm__
	// To find this offset, go into the function Caver::CharacterView::InitWithGameState and find
	// the string "Attack". In the disassembly, go up a couple instructions from the `bl` and find
	// the `ldr`. Follow the "DAT" referenced by the `ldr` instruction, and it will lead to a table
	// entry. Use the offset and value of this referenced entry. Remember to swap endianness of the value.
	// Increment the value by 1 to shift the string.

	uint32_t new_value = 0x0016F3F5;
	write_in_library(0x2bc350, &new_value, 4);

#elif defined(__aarch64__)
	// To find this offset, go into the function Caver::CharacterView::InitWithGameState and find
	// the string "Attack". In the disassembly, go up a couple instructions from the `bl` and find
	// the `add` right after the `adrp`.
	// Get the offset of that `add` instruction, and also replicate the operands in this call.
	// Increment the immediate value by 1 to shift the string.

	uint32_t new_instruction = emit_add_immediate(true, false, 0xef7, 1, 1);
	LOGD("ADD: %x", new_instruction);
	write_in_library(0x3fa530, &new_instruction, sizeof(new_instruction));

#endif

	LOGD("Applied Attack UI patch");
}

void setup_ui_health_patch() {
	// See "setup_ui_attack_patch" above.

#ifdef __arm__
	uint32_t new_value = 0x0016f3b4;
	write_in_library(0x2bc348, &new_value, 4);

#elif defined(__aarch64__)
	uint32_t new_instruction = emit_add_immediate(true, false, 0x338, 1, 1);
	LOGD("ADD: %x", new_instruction);
	write_in_library(0x3fa44c, &new_instruction, sizeof(new_instruction));

#endif

	LOGD("Applied Health UI patch");
}

bool cstrings_apply_workarounds(const char *source, const char *replacement) {
	if (strcmp(source, "Attack") == 0) {
		cstrings_add_replacement("ttack", replacement);
		setup_ui_attack_patch();
		return true;
	} else if (strcmp(source, "Health") == 0) {
		cstrings_add_replacement("ealth", replacement);
		setup_ui_health_patch();
		return true;
	}

	return false;
}
