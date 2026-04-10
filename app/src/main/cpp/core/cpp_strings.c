#include <malloc.h>
#include "cpp_strings.h"
#include "core/log.h"
#include "hooks.h"
#include "init/core.h"

#define LOG_TAG "MiniCPPStrings"

void (*CppString_create)(CppString **out, const char *data);

//void (*CppString_unsafe_release)(CppString_Header *string);

CppString_Header *CppString_s_empty;

/* Releasing a string requires special care. This uses ** to match the 32bit version. */
inline void CppString_release(CppString *string) {
	CppString_Header *base = ADJ(string);
	if (ADJ(string) == CppString_s_empty) return;

	uint32_t old, new, status;
	asm volatile (
#ifdef __aarch64__
		"loop_start:\n"
		"    ldaxr %w0, [%3]\n"         // w9 = *x8;
		"    sub %w1, %w0, #0x1\n"      // w10 = w9 - 1;
		"    stlxr %w2, %w1, [%3]\n"    // w11 = status; *x8 = w10;
		"    cbnz %w2, loop_start\n"    // if (w11 != 0) goto loop_start; // 0 means success.
#elif defined(__arm__)
		"    dmb sy\n"                  // Gain barrier
		"loop_start:\n"
		"    ldrex %0, [%3]\n"          // r9 = *r8
		"    subs %1, %0, #1\n"         // r10 = r9 - 1;
		"    strex %2, %1, [%3]\n"      // r11 = status; *r8 = r10;
		"    cmp.w %2, #0\n"
		"    bne loop_start\n"          // if (r11 != 0) goto loop_start; // 0 means success.
		// else, decrement completed...
		"    dmb sy\n"                  // Drop barrier
#endif
		: /* Outputs */ "=&r"(old), "=&r"(new), "=&r"(status)
		: /* Inputs */ "r"(&base->uc)
		: /* Clobbers */ "memory", "cc"
		);

	if (old < 1) {
		free(base);
	}
}

void initC_cpp_strings(void) {
	LOGD("Initializing Cpp Strings engine.");

	/* Fetch sentinel */
	CppString_s_empty = engine_bss_offset_ptr(BOFF_CPP_STRING_EMPTY_SENTINEL);

	/* Fetch Creation and Unsafe Release functions, because they exist on both architectures. */
	CppString_create = engine_offset_func(EOFF_CPP_STRING_FROM_CHAR_P);

//	CppString_unsafe_release = engine_offset_func(EOFF_CPP_STRING_UNSAFE_RELEASE);
}

