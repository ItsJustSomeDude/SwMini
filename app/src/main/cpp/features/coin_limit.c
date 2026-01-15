#include "coin_limit.h"

#include "../log.h"
#include "../assembly/thumb.h"
#include "../hooks.h"
#include "../assembly/arm64.h"
#include <jni.h>

#define LOG_TAG "MiniCoinLimit"

unsigned short default_coin_limit = 999;
unsigned short current_coin_limit = 999;

unsigned short current_too_rich = 999;

void reset_coin_limit() {
	set_coin_limit(default_coin_limit);
}

void set_coin_limit(unsigned short limit) {
	if (current_coin_limit == limit) return;
	current_coin_limit = limit;

#ifdef __arm__
	// Lua:
	// 0027fce0 40 f2 e7 31     movw               r1, #0x3e7

	uint32_t new_inst1 = emit_movw_t3(1, limit);
	LOGD("MOVW: %X", new_inst1);
	write_in_library(0x27fce0, &new_inst1, 4);

	// Game Event:
	// 0027e656 40 f2 e7 30     movw               r0, #0x3e7

	uint32_t new_inst2 = emit_movw_t3(0, limit);
	LOGD("MOVW: %X", new_inst2);
	write_in_library(0x27e656, &new_inst2, 4);

#elif defined(__aarch64__)
	// Lua:
	// 0035e25c 1f 9c 0f 71     cmp                w0,#0x3e7
	// 0035e260 e8 7c 80 52     mov                w8,#0x3e7

	// We actually need to change this. CMP can only be used with 12bit values.
	// We can do the MOV first to insert the imm16 into w8. Then we can compare w0 with w8 after.

	// mov  w8, #0x3e7
	// cmp  w0, w8

	uint32_t new_inst1 = emit_mov_wide_immediate(0, 0, limit, 8);
	LOGD("MOV: %X", new_inst1);
	write_in_library(0x35e25c, &new_inst1, 4);

	uint32_t new_inst2 = emit_cmp_shifted_register(0, 0, 8, 0, 0);
	LOGD("CMP: %X", new_inst2);
	write_in_library(0x35e260, &new_inst2, 4);

	// Game Event:
	// 003597a0 1f 9d 0f 71     cmp                w8,#0x3e7
	// 003597a4 ea 7c 80 52     mov                w10,#0x3e7

	// Change it the same as above.
	// mov  w10, #0x3e7
	// cmp  w8, w10

	uint32_t new_inst3 = emit_mov_wide_immediate(0, 0, limit, 10);
	LOGD("MOV: %X", new_inst3);
	write_in_library(0x3597a0, &new_inst3, 4);

	uint32_t new_inst4 = emit_cmp_shifted_register(0, 8, 10, 0, 0);
	LOGD("CMP: %X", new_inst4);
	write_in_library(0x3597a4, &new_inst4, 4);

#endif

	LOGD("Set coin limit to %i", limit);
}

void set_too_rich_amount(unsigned short amount) {
	if (current_too_rich == amount) return;
	current_too_rich = amount;

#ifdef __arm__
	// Too Rich, Game Event:
	// 0027e666 40 f2 e6 31     movw.gt            r1, #0x3e6
	// The .gt is the previous instruction, so we can ignore it in the re-assembly.

	uint32_t new_inst = emit_movw_t3(1, amount - 1);
	write_in_library(0x27e666, &new_inst, 4);

#elif defined(__aarch64__)
	// Too Rich, Game Event:
	// 003597ac 1f 01 0b 6b     cmp                w8,w11
	// <irrelevant instruction>
	// 003597b4 0d 02 00 54     b.le               switchD_00459380::caseD_5
	// 003597b8 1f 99 0f 71     cmp                w8,#0x3e6

	// We have to change the logic here. Here's what vanilla does:
	// if ((currentCoins < newCoins) && (998 < newCoins)) { /* Give Achievement */ }
	// This means if you're already at the coin limit and you pick one up, you won't get the achievement.

	// Here's our replacement:
	// if (998 < newCoins) { /* Give Achievement */ }

	// The `cmp` and `b.le` instructions are responsible for this. We'll replace the first with NOP,
	// the second with a MOV into register, and the third with a CMP using the register.
	// After this, if the limit is 999 and you're at the limit (by Lua), picking up a coin will
	// give the achievement (even though the counter does not increase.)

	// We need this so we can do a Register compare, instead of an Immediate compare,
	// which is limited to 12bits.

	// nop
	// ...
	// mov  w11, #0x3e6
	// cmp  w8, w11

	uint32_t new_inst1 = emit_nop();
	LOGD("NOP: %X", new_inst1);
	write_in_library(0x3597ac, &new_inst1, 4);

	uint32_t new_inst2 = emit_mov_wide_immediate(0, 0, amount - 1, 11);
	LOGD("MOV: %X", new_inst2);
	write_in_library(0x3597b4, &new_inst2, 4);

	uint32_t new_inst3 = emit_cmp_shifted_register(0, 8, 11, 0, 0);
	LOGD("CMP: %X", new_inst3);
	write_in_library(0x3597b8, &new_inst3, 4);

#endif

	LOGD("TooRich amount set to %i", amount);
}

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_NativeBridge_setDefaultCoinLimit(
	JNIEnv *env, jclass clazz, jint limit
) {
	// Use the bottom 16 bits of the Java int.
	unsigned short a = limit & 0xFFFF;

	default_coin_limit = a;
	set_coin_limit(a);
}

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_NativeBridge_setTooRichAmount(
	JNIEnv *env, jclass clazz, jint amount
) {
	// Use the bottom 16 bits of the Java int.
	unsigned short a = amount & 0xFFFF;

	// Use the bottom 16 bits of the Java int.
	set_too_rich_amount(a);
}

void init_feature_coin_limit() {
	//
}
