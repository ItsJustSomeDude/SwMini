#ifdef __aarch64__

#include <malloc.h>
#include "achievements.h"
#include "hooks.h"
#include "features/cstrings/cstrings.h"
#include "log.h"
#include "assembly/arm64.h"

#define LOG_TAG "MiniAchievements"

DL_FUNCTION_SYMBOL(
	achievement_init,
	"_ZN5Caver11AchievementC2ERKSsS2_S2_iS2_i",
	void,
	(Achievement * this, CppString * *id, CppString * *name,
		CppString * *description, int points, CppString **counter, int threshold)
)

// This is a test structure used to help debug the stack.
__attribute__((unused)) struct Test {
	// for i = 0, 232 do print("long _" .. i .. ";\t// offset = " .. string.format('%x', i * 8)) end

	void *_0;    // offset = 0
	void *_1;    // offset = 8

	void ***A_______________;     // offset = 10, read first. First call: NULL
	// First:  NULL*
	// Second: **<previous counter>
	// Third:  **<previous counter>

	void *_3;    // offset = 18
	void ***B_______________;     // offset = 20, read first. First call: NULL
	// First:  NULL*
	// Second: NULL*
	// Third:  NULL*

	long **C_______________;    // offset = 28, read first. First call: 15
	// First:  <small number>*
	// Second: 1*
	// Third:  1*

	long **D_______________;    // offset = 30, read first. First call: (random number)
	// First:  <junk>*
	// Second: <junk>*
	// Third:  <junk>*

	char *name;                 // offset = 38, read first. First call: Name
	// First:  Scavenger
	// Second: treasures
	// Third:  beammeupscotty

	void **F_______________;    // offset = 40, written first. First call: *<Wrong ID>
	// First:  "Too Fast"*
	// Second: scavenger**
	// Third:  beammeupscotty

	void *G_______________;      // offset = 48, read first. Holds junk at start?
	// First:  junk
	// Second: scavenger*
	// Third:  beammeupscotty

	char *H_______________;     // offset = 50, read first. Holds id at start.
	// First:  scavenger
	// Second: treasures
	// Third:  beammeupscotty

	void *I_______________;      // offset = 58, written first. First call: 3
	// First:  3
	// Second: scavenger
	// Third:  beammeupscotty

	void ***J_______________;     // offset = 60, read first. First call: NULL
	// First:  NULL
	// Second: junk**
	// Third:  junk**

	Achievement *achievement;     // offset = 68, written first. Holds id after being written.
	// First:  junk*
	// Second: <previous achievement>
	// Third:  current achievement?

	void **L_______________;    // offset = 70, read first. First call: *null
	// First:  NULL**
	// Second: junk**
	// Third:  junk**

	// End of structure thing
	// First:  treasures
	// Second: treasures
	// Third:  beammeupscotty

	void *_15;    // offset = 78
	void *_16;    // offset = 80
	void *_17;    // offset = 88
	void *_18;    // offset = 90
	void *_19;    // offset = 98
	void *_20;    // offset = a0
	void *_21;    // offset = a8
	void *_22;    // offset = b0
	void *_23;    // offset = b8
	void *_24;    // offset = c0
	void *_25;    // offset = c8
	void *_26;    // offset = d0
	long _27;    // offset = d8
	long _28;    // offset = e0
	long _29;    // offset = e8
	long _30;    // offset = f0
	long _31;    // offset = f8
	long _32;    // offset = 100
	long _33;    // offset = 108
	long _34;    // offset = 110
	long _35;    // offset = 118
	long _36;    // offset = 120
	long _37;    // offset = 128
	long _38;    // offset = 130
	long _39;    // offset = 138
	long _40;    // offset = 140
	long _41;    // offset = 148
	long _42;    // offset = 150
	long _43;    // offset = 158
	long _44;    // offset = 160
	long _45;    // offset = 168
	long _46;    // offset = 170
	long _47;    // offset = 178
	long _48;    // offset = 180
	long _49;    // offset = 188
	long _50;    // offset = 190
	long _51;    // offset = 198
	long _52;    // offset = 1a0
	long _53;    // offset = 1a8
	long _54;    // offset = 1b0
	long _55;    // offset = 1b8
	long _56;    // offset = 1c0
	long _57;    // offset = 1c8
	long _58;    // offset = 1d0
	long _59;    // offset = 1d8
	long _60;    // offset = 1e0
	long _61;    // offset = 1e8
	long _62;    // offset = 1f0
	long _63;    // offset = 1f8
	long _64;    // offset = 200
	long _65;    // offset = 208
	long _66;    // offset = 210
	long _67;    // offset = 218
	long _68;    // offset = 220
	long _69;    // offset = 228
	long _70;    // offset = 230
	long _71;    // offset = 238
	long _72;    // offset = 240
	long _73;    // offset = 248
	long _74;    // offset = 250
	long _75;    // offset = 258
	long _76;    // offset = 260
	long _77;    // offset = 268
	long _78;    // offset = 270
	long _79;    // offset = 278
	long _80;    // offset = 280
	long _81;    // offset = 288
	long _82;    // offset = 290
	long _83;    // offset = 298
	long _84;    // offset = 2a0
	long _85;    // offset = 2a8
	long _86;    // offset = 2b0
	long _87;    // offset = 2b8
	long _88;    // offset = 2c0
	long _89;    // offset = 2c8
	long _90;    // offset = 2d0
	long _91;    // offset = 2d8
	long _92;    // offset = 2e0
	long _93;    // offset = 2e8
	long _94;    // offset = 2f0
	long _95;    // offset = 2f8
	long _96;    // offset = 300
	long _97;    // offset = 308
	long _98;    // offset = 310
	long _99;    // offset = 318
	long _100;    // offset = 320
	long _101;    // offset = 328
	long _102;    // offset = 330
	long _103;    // offset = 338
	long _104;    // offset = 340
	long _105;    // offset = 348
	long _106;    // offset = 350
	long _107;    // offset = 358
	long _108;    // offset = 360
	long _109;    // offset = 368
	long _110;    // offset = 370
	long _111;    // offset = 378
	long _112;    // offset = 380
	long _113;    // offset = 388
	long _114;    // offset = 390
	long _115;    // offset = 398
	long _116;    // offset = 3a0
	long _117;    // offset = 3a8
	long _118;    // offset = 3b0
	long _119;    // offset = 3b8
	long _120;    // offset = 3c0
	long _121;    // offset = 3c8
	long _122;    // offset = 3d0
	long _123;    // offset = 3d8
	long _124;    // offset = 3e0
	long _125;    // offset = 3e8
	long _126;    // offset = 3f0
	long _127;    // offset = 3f8
	long _128;    // offset = 400
	long _129;    // offset = 408
	long _130;    // offset = 410
	long _131;    // offset = 418
	long _132;    // offset = 420
	long _133;    // offset = 428
	long _134;    // offset = 430
	long _135;    // offset = 438
	long _136;    // offset = 440
	long _137;    // offset = 448
	long _138;    // offset = 450
	long _139;    // offset = 458
	long _140;    // offset = 460
	long _141;    // offset = 468
	long _142;    // offset = 470
	long _143;    // offset = 478
	long _144;    // offset = 480
	long _145;    // offset = 488
	long _146;    // offset = 490
	long _147;    // offset = 498
	long _148;    // offset = 4a0
	long _149;    // offset = 4a8
	long _150;    // offset = 4b0
	long _151;    // offset = 4b8
	long _152;    // offset = 4c0
	long _153;    // offset = 4c8
	long _154;    // offset = 4d0
	long _155;    // offset = 4d8
	long _156;    // offset = 4e0
	long _157;    // offset = 4e8
	long _158;    // offset = 4f0
	long _159;    // offset = 4f8
	long _160;    // offset = 500
	long _161;    // offset = 508
	long _162;    // offset = 510
	long _163;    // offset = 518
	long _164;    // offset = 520
	long _165;    // offset = 528
	long _166;    // offset = 530
	long _167;    // offset = 538
	long _168;    // offset = 540
	long _169;    // offset = 548
	long _170;    // offset = 550
	long _171;    // offset = 558
	long _172;    // offset = 560
	long _173;    // offset = 568
	long _174;    // offset = 570
	long _175;    // offset = 578
	long _176;    // offset = 580
	long _177;    // offset = 588
	long _178;    // offset = 590
	long _179;    // offset = 598
	long _180;    // offset = 5a0
	long _181;    // offset = 5a8
	long _182;    // offset = 5b0
	long _183;    // offset = 5b8
	long _184;    // offset = 5c0
	long _185;    // offset = 5c8
	long _186;    // offset = 5d0
	long _187;    // offset = 5d8
	long _188;    // offset = 5e0
	long _189;    // offset = 5e8
	long _190;    // offset = 5f0
	long _191;    // offset = 5f8
	long _192;    // offset = 600
	long _193;    // offset = 608
	long _194;    // offset = 610
	long _195;    // offset = 618
	long _196;    // offset = 620
	long _197;    // offset = 628
	long _198;    // offset = 630
	long _199;    // offset = 638
	long _200;    // offset = 640
	long _201;    // offset = 648
	long _202;    // offset = 650
	long _203;    // offset = 658
	long _204;    // offset = 660
	long _205;    // offset = 668
	long _206;    // offset = 670
	long _207;    // offset = 678
	long _208;    // offset = 680
	long _209;    // offset = 688
	long _210;    // offset = 690
	long _211;    // offset = 698
	long _212;    // offset = 6a0
	long _213;    // offset = 6a8
	long _214;    // offset = 6b0
	long _215;    // offset = 6b8
	long _216;    // offset = 6c0
	long _217;    // offset = 6c8
	long _218;    // offset = 6d0
	long _219;    // offset = 6d8
	long _220;    // offset = 6e0
	long _221;    // offset = 6e8
	long _222;    // offset = 6f0
	long _223;    // offset = 6f8
	long _224;    // offset = 700
	long _225;    // offset = 708
	long _226;    // offset = 710
	long _227;    // offset = 718
	long _228;    // offset = 720
	long _229;    // offset = 728
	long _230;    // offset = 730
	long _231;    // offset = 738
	char *Counter;    // offset = 740
} TestStruct;
// Some helpful Watches for LLDB:
// *((struct Test*)$sp)
// $sp
// ((struct Test*)$sp)->Counter
// (Achievement**)$x2

void create_achievement(
	void *manager,
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

//	uintptr_t frame_size = 0x740;           // bytes, ≥ 0, multiple of 16
	void *target_func = offset_address(0x377670);

	asm volatile (
		// Store all the things in registers, I'm about to clobber the whole stack!
		"mov x19, %0\n\t" // manager
		"mov x0, %1\n\t"  // name pointer
		"mov x1, %2\n\t"  // id pointer
		"mov x2, %3\n\t"  // counter pointer

		"mov x22, %4\n\t" // input achievement
		"mov x21, %5\n\t" // x21 = empty_basic_string - 3

		"mov x27, #0x0\n\t" // Loop Offset (0 because we're only registering 1 achievement)
		"mov x20, #0x30\n\t" // Size of achievement

		// All the registers are ready. Make a stack frame.
		"sub sp, sp, #0x740\n\t"

		// Shared Count Location. Vanilla uses 2 instructions and a register, I can do it in 1.
		// x8 is written first by the target method.
		// "add x8, sp, #0x68\n\t"
		// "add x24, x8, #0x8\n\t"
		"add x24, sp, #0x70\n\t"

		// Really inefficiently put a bunch of stuff on the stack...
		"add x23, x19, #0x48\n\t"  // manager+0x48 -> stack+0x10 -> stack+0x20
		"str x23, [sp, #0x10]\n\t"
		"str x23, [sp, #0x20]\n\t"

		"add x23, x19, #0x18\n\t"  // manager+0x18 -> stack+0x28
		"str x23, [sp, #0x28]\n\t"

		"add x23, x19, #0x60\n\t"  // manager+0x60 -> stack+0x30
		"str x23, [sp, #0x30]\n\t"

		"str x0, [sp, #0x38]\n\t"  // name* -> stack+0x38
		"str x1, [sp, #0x50]\n\t"  // id* -> stack+0x50
		"str x2, [sp, #0x740]\n\t" // counter* -> stack+0x740

		// Place the return address into x28, as it appears it is never used.
		"adr x28, after_call\n\t"        // Load pc into x28

		// Stack is ready! JUMP!
		"blr %6\n\t"

		"after_call:\n\t"
		// Remove the stack frame.
		"add sp, sp, #0x740\n\t"

		: // No outputs...
		: "r"(manager), "r"(cpp_name), "r"(cpp_id), "r"(cpp_counter),
	"r"(&input_achievement),
	/* Empty Basic String pointer in BSS. Cannot be a local variable due to stack destruction. */
	"r"(engine_bss_offset_ptr(0x14880)),
	"r"(target_func)
		: "x19", "x0", "x1", "x2", "x22", "r24", "r21", "x27", "x20", "x23", "x28", "memory"
		);

	// Stack frame was cleaned up already. ALL registers should be considered clobbered, but since
	// I'm just returning from here it should be ok.
	// If I get crashes here, then more registers will need to be added to the Clobbered list.
}

void *shared_achievement_manager = NULL;

STATIC_DL_HOOK_SYMBOL(
	achievement_manager_init,
	"_ZN5Caver19AchievementsManagerC1Ev",
	void,
	(void *this)
) {
	LOGD("Creating shared achievement manager at %p", this);

	orig_achievement_manager_init(this);
	shared_achievement_manager = this;

	ach_register_all();

//	create_achievement(this, "test1", "Test Achievement", "This is the achievement!", 10, NULL, 1);
//	create_achievement(this, "test2", "A Second Test", "This is another one.", 5, "cc", 5);

	LOGD("Done. %p", this);
}

void init_feature_achievements() {
	dlsym_achievement_init();
	hook_achievement_manager_init();

	// Skip over vanilla achievement initialization.
	branch_within_engine(0x376088, 0x377ba8, false);

	// Add a `ret x28` instruction to the end of the `while` loop body.
	uint32_t ret = emit_ret(28);
	write_in_library(0x377a04, &ret, 4);
}

#endif
