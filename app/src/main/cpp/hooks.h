#ifndef SWMINI_HOOKS_H
#define SWMINI_HOOKS_H

#include <stdbool.h>
#include <stddef.h>

extern void *engine_dl_handle;
extern void *engine_load_bias;
extern void *engine_bss_start;

__attribute__((unused))
void *hook_symbol(const char *symName, void *newAddr, void **origAddr);
void *hook_address(void *func_addr, void *new_addr, void **orig_addr);
__attribute__((unused))
void *hook_engine_offset(unsigned long offset, void *new_addr, void **orig_addr);
void *engine_symbol_ptr(const char *symbol);
void *engine_offset_ptr(unsigned int offset);
void *engine_bss_offset_ptr(unsigned int offset);

void *branch_within_engine(long from, long to, bool use_small_instruction);

void write_in_library(long offset, void *data, size_t size);

void setup_hooks();

/**
 * This file defines macros to help call and hook functions from dynamically loaded libraries.
 */

#if defined(__aarch64__)
/* Use first or second argument, based on 32- or 64-bit build */
#define archSplit(b32, b64) b64
#elif defined(__arm__)
/* Use first or second argument, based on 32- or 64-bit build */
#define archSplit(b32, b64) b32
#endif

/** Dynamic pointer offset per architecture. Move n pointers ahead. */
#define pointerOffset(n) (n * sizeof(void*))

#define $(type, base, b32, b64) \
    ((type*)(base + archSplit(b32, b64)))

/**
 * Used to make a header for any function from a dynamic library, in an H file.
 * Defines headers for the Type Name, a pointer to the function, and a function to dlsym the symbol.
 *
 * Once setup in an H file, use DL_FUNCTION_SYMBOL or DL_FUNCTION_OFFSET in a C file.
 *
 * @param name User-friendly name for this symbol
 * @param ret Return type of function
 * @param params Parameters of function, (in parenthesis)
 */
#define H_DL_FUNCTION(name, ret, params)                \
typedef ret type_##name params;                         \
extern type_##name *name;                               \
inline void* dlsym_##name();

/**
 * Defines a dynamic function given a Symbol Name, using types from a previous H_DL_FUNCTION expansion.
 * Creates a pointer to the function, and a function to dlsym the symbol.
 *
 * @param name Function identifier matching a previous H_DL_FUNCTION expansion
 * @param symbol String holding the mangled symbol name
 *
 * @see H_DL_FUNCTION
 */
#define DL_FUNCTION_SYMBOL(name, symbol, ...)           \
type_##name *name;                                      \
inline __attribute__((always_inline))                   \
void* dlsym_##name() {                                  \
    return name = engine_symbol_ptr(symbol);            \
}

/**
 * Defines a dynamic function given a load bias offset, using types from a previous H_DL_FUNCTION expansion.
 * Creates a pointer to the function, and a function to "dlsym" the symbol.
 *
 * @param name Function identifier matching a previous H_DL_FUNCTION expansion
 * @param offset Unsigned long, offset into shared library.
 */
#define DL_FUNCTION_OFFSET(name, offset, ...)           \
type_##name *name;                                      \
inline __attribute__((always_inline))                   \
void* dlsym_##name() {                                  \
    return name = engine_offset_ptr(offset);            \
}

/**
 * Defines a static function from a dynamic library, given a symbol name.
 * Static version, not to be used across files.
 *
 * @param name User-friendly name for this symbol
 * @param sym String holding the mangled symbol name
 * @param ret Return type of function
 * @param params Parameters of function, (in parenthesis)
 */
#define STATIC_DL_FUNCTION_SYMBOL(name, sym, ret, params) \
static ret (*name) params;                                \
inline __attribute__((always_inline))                     \
static void* dlsym_##name() {                             \
    return name = engine_symbol_ptr(sym);                 \
}


/**
 * Defines a static function from a dynamic library, given a function byte offset.
 * Static version, not to be used across files.
 *
 * @param name User-friendly name for this symbol
 * @param offset Unsigned long, offset into shared library.
 * @param ret Return type of function
 * @param params Parameters of function, (in parenthesis)
 */
#define STATIC_DL_FUNCTION_OFFSET(name, offset, ret, params) \
static ret (*name) params;                                   \
inline __attribute__((always_inline))                        \
static void* dlsym_##name() {                                \
    return name = engine_offset_ptr(offset);                 \
}

/**
 * This is used to define the header boilerplate for hooking a function.
 * The symbol MUST already be defined with H_DL_FUNCTION, and the passed name must match what was
 * passed to that macro.
 *
 * It creates a pointer to the original function, a pointer to the stub (after hooked),
 * and a function to initialize the hook.
 *
 * @param name User-friendly name for this hook, matching a previous H_DL_FUNCTION expansion.
 *
 */
#define H_DL_HOOK(name, ...)                            \
extern type_##name *orig_##name;                        \
extern void *stub_##name;                               \
inline void *hook_##name();

/**
 * This is used to define the combined header boilerplate for a function and its hook.
 *
 * @param name Name of the function.
 * @param ret Return type of the function.
 * @param params Params of the function, (in parenthesis).
 *
 * @see H_DL_FUNCTION
 * @see H_DL_HOOK
 */
#define H_DL_FUNCTION_HOOK(name, ret, params)           \
    H_DL_FUNCTION(name, ret, params)                    \
    H_DL_HOOK(name)

/** @internal */
#define _define_hook(name, ret, params, hf, hp)         \
static type_##name hooked_##name;                       \
type_##name *orig_##name;                               \
void *stub_##name;                                      \
void* hook_##name() {                                   \
    return stub_##name = hf(                            \
        hp, hooked_##name, (void**)&orig_##name         \
    );                                                  \
}                                                       \
static ret hooked_##name params

/** @internal */
#define _define_static_hook(name, ret, params, hf, hp)  \
static ret hooked_##name params;                        \
static ret (*orig_##name) params;                       \
static void* stub_##name;                               \
static void* hook_##name() {                            \
    return stub_##name = hf(                            \
        hp, hooked_##name, (void**)&orig_##name         \
    );                                                  \
}                                                       \
static ret hooked_##name params



/**
 * Create a function hook from a dynamic symbol name.
 *
 * @param name Function identifier matching a previous H_DL_HOOK expansion
 * @param symbol String holding the mangled symbol name
 * @param ret Return type of function
 * @param params Params of the function, (in parenthesis).
 */
#define DL_HOOK_SYMBOL(name, symbol, ret, params)       \
    _define_hook(name, ret, params, hook_symbol, symbol)

/**
 * Create a function hook from a pointer offset.
 *
 * @param name Function identifier matching a previous H_DL_HOOK expansion
 * @param offset Offset into library of function to hook
 * @param ret Return type of function
 * @param params Params of the function, (in parenthesis).
 */
#define DL_HOOK_OFFSET(name, offset, ret, params)       \
    _define_hook(name, ret, params, hook_engine_offset, offset)

/**
 * Create a function hook from an address.
 *
 * @param name Function identifier matching a previous H_DL_HOOK expansion
 * @param addr Address of function to hook
 * @param ret Return type of function
 * @param params Params of the function, (in parenthesis).
 */
#define DL_HOOK_ADDR(name, addr, ret, params)           \
    _define_hook(name, ret, params, hook_address, addr)

/**
 * Create a function hook from a dynamic symbol name, standalone in one file.
 * Follow this expansion with the hooked function body.
 *
 * @param name Function identifier
 * @param symbol String holding the mangled symbol name
 * @param ret Return type of function
 * @param params Params of the function, (in parenthesis).
 */
#define STATIC_DL_HOOK_SYMBOL(name, symbol, ret, params)    \
    _define_static_hook(name, ret, params, hook_symbol, symbol)

/**
 * Create a function hook from a library offset, standalone in one file.
 * Follow this expansion with the hooked function body.
 *
 * @param name Function identifier
 * @param offset Offset into library of function to hook
 * @param ret Return type of function
 * @param params Params of the function, (in parenthesis).
 */
#define STATIC_DL_HOOK_OFFSET(name, offset, ret, params)    \
    _define_static_hook(name, ret, params, hook_engine_offset, offset)

/**
 * Create a function hook from a runtime address, standalone in one file.
 * Follow this expansion with the hooked function body.
 *
 * @param name Function identifier
 * @param addr Address of function to hook
 * @param ret Return type of function
 * @param params Params of the function, (in parenthesis).
 */
#define STATIC_DL_HOOK_ADDR(name, addr, ret, params)    \
    _define_static_hook(name, ret, params, hook_address, addr)

#endif //SWMINI_HOOKS_H
