
// This file is to help define Caver symbols in C / H files.

#ifndef SWMINI_SYMBOL_H
#define SWMINI_SYMBOL_H

#include "hook_core.h"

#if defined(__aarch64__)
/* Dynamic pointer offset per architecture. Move n pointers ahead. */
#define pointerOffset(n) (n * 8)
#define splitOffset(b32, b64) (b64)
#define splitLiteral(b32, b64) b64
#elif defined(__arm__)
/* Dynamic pointer offset per architecture. Move n pointers ahead. */
#define pointerOffset(n) (n * 4)
#define splitOffset(b32, b64) (b32)
#define splitLiteral(b32, b64) b32
#endif



//
// Header file macros
//

// This is used to define any symbol from LS, in an H file.
#define DEFINE_SYMBOL_H(name, ret, params)              \
typedef ret type_##name params;                         \
extern type_##name *name;                               \
inline void* dlsym_##name();

// This is used to define hooks a function in an H file.
#define DEFINE_HOOK_H(name, ...)                        \
extern type_##name *orig_##name;                        \
extern void *stub_##name;                               \
inline void *hook_##name();

// Defines the Symbol and Hooks in an H file.
#define DEFINE_SYMBOL_HOOK_H(name, ret, params)         \
    DEFINE_SYMBOL_H(name, ret, params)                  \
    DEFINE_HOOK_H(name)

//
// C file macros
//

// Creates a symbol matching an H file definition, using a dlsym name.
#define DEFINE_SYMBOL_SYM(name, symbol, ...)            \
type_##name *name;                                      \
/* inline */ void* dlsym_##name() {                           \
    return name = getSym(symbol);                       \
}

// Creates a symbol matching an H file definition, using a byte offset.
#define DEFINE_OFFSET_SYM(name, offset, ...)            \
type_##name *name;                                      \
inline void* dlsym_##name() {                           \
    return name = getOffset(offset);                    \
}

#define symbol_hook(name, ret, params, hf, hp)         \
static type_##name hooked_##name;                       \
type_##name *orig_##name;                               \
void *stub_##name;                                      \
extern void* hook_##name() {                            \
    return stub_##name = hf(                            \
        hp, hooked_##name, (void**)&orig_##name         \
    );                                                  \
}                                                       \
static ret hooked_##name params

#define DEFINE_HOOK_SYM(name, symbol, ret, params)      \
    symbol_hook(name, ret, params, hookSym, symbol)

#define DEFINE_HOOK_OFFSET(name, offset, ret, params)   \
    symbol_hook(name, ret, params, hookOffset, offset)

#define DEFINE_HOOK_ADDR(name, addr, ret, params)       \
    symbol_hook(name, ret, params, hookAddr, addr)

#endif //SWMINI_SYMBOL_H
