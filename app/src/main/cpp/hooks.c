#include <stdio.h>
#include <string.h>
#include <android/log.h>
#include <jni.h>
#include <dlfcn.h>

#include "hooks.h"
#include "libs/Gloss.h"
#include "log.h"
#include "hooks.h"

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
void *redirect_within_library(long from, long to, bool use4byte) {
	LOGD("Redirecting within: from: %p", libSwBase + from);
	// void* addr, void* new_addr, bool is_4_byte_hook, i_set mode
	return GlossHookRedirect(
		libSwBase + from,
		libSwBase + to,
		use4byte,
		archSplit(I_THUMB, I_ARM64)
	);
}

void setup_hooks() {
	libSwHandle = dlopen("libswordigo.so", RTLD_NOLOAD);
	libSwBase = (void *) GlossFindLibMapping("libswordigo.so", -1, libSwPath, &libSwLength);
}
