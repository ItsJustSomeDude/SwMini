#ifdef __arm__

#include "achievements.h"
#include "hooks.h"
#include "log.h"
#include "assembly/thumb.h"

#define LOG_TAG "MiniAchievements"

DL_FUNCTION_SYMBOL(
	achievement_init,
	"_ZN5Caver11AchievementC2ERKSsS2_S2_iS2_i",
	void,
	(Achievement * this, CppString * *id, CppString * *name,
		CppString * *description, int points, CppString **counter, int threshold)
)

static void *shared_achievement_manager = NULL;

void create_achievement(
	const char *id, const char *name,
	const char *desc, int points,
	const char *counter, int threshold
) {
	CppString *cpp_id = NULL;
	CppString *cpp_name = NULL;
	CppString *cpp_desc = NULL;
	CppString *cpp_counter = NULL;

	create_basic_string(&cpp_id, id);
	create_basic_string(&cpp_name, name);
	create_basic_string(&cpp_desc, desc);
	if (counter == NULL) {
		create_basic_string(&cpp_counter, "");
	} else {
		create_basic_string(&cpp_counter, counter);
	}

	// Allocate the achievement on the stack
	Achievement input_achievement = {0};
	achievement_init(
		&input_achievement,
		&cpp_id, &cpp_name, &cpp_desc, points,
		&cpp_counter, threshold
	);

	void *target_func = engine_offset_ptr(0x289ef6);

	// Stack Frame size (hardcoded in asm to save a register): #0x3c0

	asm volatile (
		// Store all the things in registers, I'm about to clobber the whole stack!
		// Registers I can use: r0, r1, r2, r3, r6, r7, r10, r12

		// Temporary registers, so I can safely clobber the stack.
		"mov r0, %0\n\t"    // manager
		"mov r1, %1\n\t"    // input achievement

		// Branch Target (loop body)
		"mov r10, %2\n\t"
		// Target engine code is always thumb mode.
		"orr r10, r10, #1\n\t"

		// All the registers are ready. Make a stack frame.
		"sub sp, sp, #0x3c0\n\t"

		// Really inefficiently put a bunch of stuff on the stack...
		// input achievement pointer -> stack+0x2c
		"str r1, [sp, #0x2c]\n\t"

		// manager -> stack+0x28
		"str r0, [sp, #0x28]\n\t"

		// manager+12 -> stack+0x20
		"add r12, r0, #12\n\t"
		"str r12, [sp, #0x20]\n\t"

		// manager+48 -> stack+0x1c
		"add r12, r0, #48\n\t"
		"str r12, [sp, #0x1c]\n\t"

		// manager+52 -> stack+0x18
		"add r12, r0, #52\n\t"
		"str r12, [sp, #0x18]\n\t"

		// manager+36 -> stack+0x14
		"add r12, r0, #36\n\t"
		"str r12, [sp, #0x14]\n\t"

		// Initialize unknowns: 0x0 -> stack+0x4c -> stack+0x3c0
		// 0x4c I have no idea what it is.
		// 0x3c0 Pointer passed to _Rb_tree<>::_M_get_insert_equal_pos, which sets it.
		"mov r12, #0x0\n\t"
		"str r12, [sp, #0x4c]\n\t"
		"str r12, [sp, #0x3c0]\n\t"

		// Set needed registers.
		"mov r4, #0x0\n\t"
		"add r5, sp, #0x3c\n\t"
		"add r8, sp, #0x3ac\n\t"
		"add r9, sp, #0x30\n\t"
		"add r11, sp, #0x4c\n\t" // 0x48 + 0x4

		// Place the return address into r7, as it appears it is never used.
		"adr r7, after_call\n\t"

#ifdef __thumb__
		// If this ASM block is thumb, set the flag so we can jump back to it safely.
		"orr r7, r7, #1\n\t"
#endif

		// Stack is ready! JUMP!
		"blx r10\n\t"

		"after_call:\n\t"
		// Remove the stack frame.
		"add sp, sp, #0x3c0\n\t"

		: // No outputs.
		: "r"(shared_achievement_manager), "r"(&input_achievement), "r"(target_func)
		: "r0", "r1", "r10", "r12", "r4", "r5", "r8", "r9", "r11", "r7", "memory"
		);

	// Stack frame was cleaned up already. ALL registers should be considered clobbered, but since
	// I'm just returning from here it should be ok.
	// If I get crashes here, then more stuff will need to be added to the Clobbered list.
}

STATIC_DL_HOOK_SYMBOL(
	achievement_manager_init,
	"_ZN5Caver19AchievementsManagerC1Ev",
	void,
	(void *this)
) {
	LOGD("Creating shared achievement manager at %p", this);

	orig_achievement_manager_init(this);
	shared_achievement_manager = this;

	miniACH_register();

//	create_achievement(this, "test1", "Test Achievement", "This is the achievement!", 10, NULL, 1);
//	create_achievement(this, "test2", "A Second Test", "This is another one.", 5, "cc", 5);

	LOGD("Done. %p", this);
}


void init_feature_achievements() {
	dlsym_achievement_init();

	hook_achievement_manager_init();

	// Skip over vanilla achievement initialization.
	branch_within_engine(0x2891aa, 0x28a0ac, false);

	// Add a `bx r7` instruction to the end of the `while` loop body.
	uint16_t inst1 = emit_bx_t1(7);
	write_in_library(0x28a06a, &inst1, 2);
}

#endif
