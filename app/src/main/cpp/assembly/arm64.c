#ifdef __aarch64__

#include <stdint.h>
#include <stdbool.h>
#include "arm64.h"

uint32_t arrange_instruction(uint32_t value) {
	return ((value >> 24) & 0x000000FF) | // Move byte 3 to byte 0
	       ((value << 8) & 0x00FF0000) | // Move byte 1 to byte 2
	       ((value >> 8) & 0x0000FF00) | // Move byte 2 to byte 1
	       ((value << 24) & 0xFF000000);   // Move byte 0 to byte 3
}

uint32_t emit_add_immediate(bool is64, bool shift, uint16_t imm12, uint8_t rn, uint8_t rd) {
	uint32_t opc = 0;

	opc |= is64 << 31;
	opc |= 0b00100010 << 23;
	opc |= shift << 22;
	opc |= imm12 << 10;
	opc |= rn << 5;
	opc |= rd;

	return opc;
}

uint32_t emit_cmp_immediate(bool is64, bool shift, uint16_t imm12, uint8_t rn) {
	uint32_t opc = 0;

	opc |= is64 << 31;
	opc |= 0b11100010 << 23;
	opc |= shift << 22;
	opc |= imm12 << 10;
	opc |= rn << 5;
	opc |= 0b11111;

	return opc;
}

uint32_t emit_cmp_shifted_register(bool is64, uint8_t rn, uint8_t rm, uint8_t shift, uint8_t imm6) {
	uint32_t opc = 0;

	opc |= is64 << 31;
	opc |= 0b1101011 << 24;
	opc |= shift << 22;
	// opc |= 0 << 21;
	opc |= rm << 16;
	opc |= imm6 << 10;
	opc |= rn << 5;
	opc |= 0b11111; // << 0

	return opc;
}

uint32_t emit_mov_wide_immediate(bool is64, uint8_t hw, uint16_t imm16, uint8_t rd) {
	uint32_t opc = 0;

	opc |= is64 << 31;
	opc |= 0b10100101 << 23;
	opc |= hw << 21;
	opc |= imm16 << 5;
	opc |= rd; // rd << 0

	return opc;
}

uint32_t emit_nop() {
	// NOP: 11010101 00000011 00100000 00011111

	uint32_t opc = 0b11010101000000110010000000011111;;

	return opc;
}

uint32_t emit_ret(uint8_t rn) {
	uint32_t opc = 0;

	opc |= 0b1101011001011111000000 << 10;
	opc |= rn << 5;
	// opc |= 0b00000 << 0;

	return opc;
}

#endif //__aarch64__
