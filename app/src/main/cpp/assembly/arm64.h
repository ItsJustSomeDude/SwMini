#ifdef __aarch64__
#ifndef NEEDLE_ARM64_H
#define NEEDLE_ARM64_H

#include <stdbool.h>
#include <stdint.h>

// Needed for CString Workaround Patch
uint32_t emit_add_immediate(bool is64, bool shift, uint16_t imm12, uint8_t rn, uint8_t rd);

// Needed for Coin Limit Patch
uint32_t emit_cmp_immediate(bool is64, bool shift, uint16_t imm12, uint8_t rn);

// Needed for Coin Limit Patch
uint32_t emit_mov_wide_immediate(bool is64, uint8_t hw, uint16_t imm16, uint8_t rd);

uint32_t emit_cmp_shifted_register(bool is64, uint8_t rn, uint8_t rm, uint8_t shift, uint8_t imm6);

uint32_t emit_nop();

uint32_t emit_ret(uint8_t rn);

#endif //NEEDLE_ARM64_H
#endif //__aarch64__
