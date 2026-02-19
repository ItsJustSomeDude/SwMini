#include "hooks.h"

#include "assembly/arm64.h"
#include "assembly/thumb.h"
#include "libs/Gloss.h"
#include "log.h"

#include <dlfcn.h>

#define LOG_TAG "MiniHookCore"

void *engine_dl_handle;
void *engine_load_bias;
void *engine_bss_start;

// This is used by the DL_HOOK_FUNCTION_SYMBOL macro.
__attribute__((unused))
void *hook_symbol(const char *symName, void *newAddr, void **origAddr) {
	void *sym = engine_symbol_ptr(symName);
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

// This is used by the DL_HOOK_FUNCTION_SYMBOL macro.
__attribute__((unused))
void *hook_engine_offset(unsigned long offset, void *new_addr, void **orig_addr) {
	return hook_address(engine_load_bias + offset, new_addr, orig_addr);
}

void *engine_symbol_ptr(const char *symbol) {
	void *ptr = dlsym(engine_dl_handle, symbol);
	if (ptr == NULL) {
		LOGE("Symbol '%s' lookup failed: %s", symbol, dlerror());
		return NULL;
	}
	return ptr;
}

void *engine_bss_offset_ptr(unsigned int offset) {
	return engine_bss_start + offset;
}

void *engine_offset_ptr(unsigned int offset) {
	return engine_load_bias + offset;
}

inline __attribute__((always_inline))
void *symbol_offset_ptr(void *sym_ptr, unsigned int offset) {
	return sym_ptr + offset;
}

void *engine_symbol_offset_ptr(char *symbol, unsigned int offset) {
	return engine_symbol_ptr(symbol) + offset;
}

void write_in_library(long offset, void *data, size_t size) {
	WriteMemory(engine_load_bias + offset, data, size, true);
}

/** Returns the stub? */
void *branch_within_engine(long from, long to, bool use_small_instruction) {
	LOGD("Redirecting within: from: %p", engine_load_bias + from);

#ifdef __arm__
	// Since the PC is located slightly differently from the actual current point, I need to subtract 4.
	long jmp_distance = to - from - 4;

	if ((jmp_distance & 1) != 0) {
		LOGE("Attempted to jump unaligned distance: %li (0x%lx)", jmp_distance, jmp_distance);
	} else if (use_small_instruction && (jmp_distance < -0x800 || jmp_distance > 0x7FE)) {
		LOGE("Jump is out of range for 2byte branch: %li (0x%lx)", jmp_distance, jmp_distance);
	} else if (use_small_instruction) {
		uint16_t opc = emit_b_t2((short) jmp_distance);

		write_in_library(from, &opc, 2);
		LOGD(
			"Inserted 2 byte unconditional Thumb branch with distance %lx at offset %lx",
			jmp_distance, from
		);
	} else if (jmp_distance < -0x1000000 || jmp_distance > 0xFFFFFE) {
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
		engine_load_bias + from,
		engine_load_bias + to,
		use_small_instruction,
		archSplit(I_THUMB, I_ARM64)
	);
#endif
}

void setup_hooks() {
	engine_dl_handle = dlopen("libswordigo.so", RTLD_NOLOAD);

	// Fetch the Library Base Offset by capturing a symbol, getting the Symbol Info, and reading the base from there.
	// We use the `.bss` table, as we need to capture that address anyway.
	// GlossHook's method is broken with MinSDK past 22.
	engine_bss_start = dlsym(engine_dl_handle, "__bss_start");
	Dl_info info;
	dladdr(engine_bss_start, &info);
	engine_load_bias = info.dli_fbase;

	LOGD("Engine Load Bias: %p, Data section: %p", engine_load_bias, engine_bss_start);
}
