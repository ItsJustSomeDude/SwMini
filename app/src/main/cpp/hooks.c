#include <stdio.h>
#include <string.h>
#include <android/log.h>
#include <jni.h>
#include <dlfcn.h>

#include "hooks.h"
#include "libs/Gloss.h"
#include "log.h"
#include "hooks.h"
#include "assembly/thumb.h"

#define LOG_TAG "MiniHookCore"

static char *libSwPath;
static size_t libSwLength;
static void *libSwHandle;
static void *libSwBase;

void *hook_symbol(const char *symName, void *newAddr, void **origAddr) {
	void *sym = symbol_address(symName);
	if (sym == NULL) {
		LOGE("Failed to hook %s: Failed to find offset.", symName);
		return NULL;
	}

	return hook_address(sym, newAddr, origAddr);
}

void *hook_address(void *func_addr, void *new_addr, void **orig_addr) {
	void *stub = GlossHookAddr(
		func_addr,
		new_addr,
		orig_addr,
		false,
		archSplit(I_THUMB, I_ARM64)
	);
	if (stub == NULL) {
		LOGE("Failed to hook address: %p", func_addr);
		return NULL;
	}
	return stub;
}

void *hook_offset(unsigned long offset, void *new_addr, void **orig_addr) {
	return hook_address(libSwBase + offset, new_addr, orig_addr);
}

void *symbol_address(const char *symbol) {
	void *ptr = dlsym(libSwHandle, symbol);
	if (ptr == NULL) {
		LOGE("Symbol '%s' lookup failed: %s", symbol, dlerror());
		return NULL;
	}
	return ptr;
}

void *offset_address(unsigned int offset) {
	return libSwBase + offset;
}

void write_in_library(long offset, void *data, size_t size) {
	WriteMemory(libSwBase + offset, data, size, true);
}

/** Returns the stub? */
void *redirect_within_library(long from, long to, bool use_small_instruction) {
	LOGD("Redirecting within: from: %p", libSwBase + from);

#ifdef __arm__
	// Since the PC is located slightly differently from the actual current point, I need to subtract 4.
	long jmp_distance = to - from - 4;

	if ((jmp_distance & 1) != 0) {
		LOGE("Attempted to jump unaligned distance: %li (0x%lx)", jmp_distance, jmp_distance);
	} else if (use_small_instruction && (jmp_distance < -2048 || jmp_distance > 2046)) {
		LOGE("Jump is out of range for 2byte branch: %li (0x%lx)", jmp_distance, jmp_distance);
	} else if (use_small_instruction) {
		uint16_t opc = emit_b_t2((short) jmp_distance);

		write_in_library(from, &opc, 2);
		LOGD(
			"Inserted 2 byte unconditional Thumb branch with distance %lx at offset %lx",
			jmp_distance, from
		);
	} else if (jmp_distance < -16777216 || jmp_distance > 16777214) {
		LOGE("Jump is out of range for branch: %li (0x%lx)", jmp_distance, jmp_distance);
	} else {
		uint32_t opc = emit_b_t4(jmp_distance);

		write_in_library(from, &opc, 4);
		LOGD(
			"Inserted 4 byte unconditional Thumb branch with distance 0x%lx at offset 0x%lx",
			jmp_distance, from
		);
	}
	return NULL;

#elif defined(__aarch64__)
	// void* addr, void* new_addr, bool is_4_byte_hook, i_set mode
	return GlossHookRedirect(
		libSwBase + from,
		libSwBase + to,
		use_small_instruction,
		archSplit(I_THUMB, I_ARM64)
	);
#endif
}

void setup_hooks() {
	libSwHandle = dlopen("libswordigo.so", RTLD_NOLOAD);
	libSwBase = (void *) GlossFindLibMapping("libswordigo.so", -1, libSwPath, &libSwLength);
	LOGD("LibSw Load Bias: %p", libSwBase);
}
