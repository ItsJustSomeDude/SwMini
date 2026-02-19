#ifdef __arm__

#include "thumb.h"

#include "utils.h"

#include <stdbool.h>
#include <stdint.h>

uint32_t arrange_4byte_instruction(uint32_t value) {
	// Extract the first and second sets of 8 bits
	uint32_t part1 = (value >> 24) & 0xFF; // 11110x10 -> first 8 bits
	uint32_t part2 = (value >> 16) & 0xFF; // 0100xxxx -> second 8 bits
	uint32_t part3 = (value >> 8) & 0xFF;  // 0xxxyyyy -> third 8 bits
	uint32_t part4 = value & 0xFF;         // xxxxxxxx -> fourth 8 bits

	// Swap the first two and the last two sets of 8 bits
	return (part2 << 24) | (part1 << 16) | (part4 << 8) | part3;
}

uint32_t arrange_thumb_4byte(uint32_t opc) {
	// Extract the first and second sets of 8 bits
	uint32_t part1 = (opc >> 24) & 0xFF; // 11110x10 -> first 8 bits
	uint32_t part2 = (opc >> 16) & 0xFF; // 0100xxxx -> second 8 bits
	uint32_t part3 = (opc >> 8) & 0xFF;  // 0xxxyyyy -> third 8 bits
	uint32_t part4 = opc & 0xFF;         // xxxxxxxx -> fourth 8 bits

	// Swap the first two and the last two sets of 8 bits
	return (part3 << 24) | (part4 << 16) | (part1 << 8) | part2;
}


uint16_t arrange_2byte_instruction(uint16_t value) {
	uint16_t part1 = (value >> 8) & 0xFF;
	uint16_t part2 = value & 0xFF;

	// Swap the first two and the last two sets of 8 bits
	return (part1 << 8) | part2;
}

uint32_t emit_movw_t3(uint8_t rd, uint16_t imm16) {
	uint16_t imm4 = (imm16 >> 12) & 0b1111; // bits 15–12
	uint16_t i = (imm16 >> 11) & 0b1;       // bit 11
	uint16_t imm3 = (imm16 >> 8) & 0b111;   // bits 10–8
	uint16_t imm8 = imm16 & 0b11111111;     // bits 7–0

	uint32_t opc = 0;

	opc |= (0b11110 << 27);
	opc |= (i << 26);
	opc |= (0b100100 << 20);
	opc |= (imm4 << 16);
	// opc |= (0b0 << 15);
	opc |= (imm3 << 12);
	opc |= (rd << 8);
	opc |= imm8; // (imm8 << 0);

	return arrange_thumb_4byte(opc);
	//return arrange_4byte_instruction(opc);
}

uint16_t emit_b_t2(int16_t target) {
	// target must be even, in range -2048..+2046 (inclusive)
	// i.e. multiple of 2, within ±2048 bytes from current PC

	if (target < -2048 || target > 2046 || (target & 1) != 0) {
		return 0;           // invalid → return 0 or handle error as needed
	}

	uint16_t imm11 = (uint16_t) (target >> 1);   // divide by 2 → 11-bit value

	uint16_t opc = 0;
	opc |= 0b11100u << 11;                      // opcode prefix
	opc |= imm11 & 0x07FF;                      // bits 10:0 = imm11

	return opc;
}

uint32_t emit_b_t4(int32_t target) {
	// Must be halfword aligned
	if (target & 1) {
		// handle error (unaligned branch)
		return 0;
	}

	// Range check: signed 25-bit immediate (shifted left by 1)
	if (target < -16777216 || target > 16777214) {
		// handle error (out of range)
		return 0;
	}

	// Shift right to remove the implicit low zero bit
	int32_t imm25 = target >> 1;

	// Extract fields
	uint32_t S = (imm25 >> 24) & 1;
	uint32_t I1 = (imm25 >> 23) & 1;
	uint32_t I2 = (imm25 >> 22) & 1;
	uint32_t imm10 = (imm25 >> 12) & 0x3FF;
	uint32_t imm11 = imm25 & 0x7FF;

	// Encode J bits (this is the critical XOR logic)
	uint32_t J1 = (~(I1 ^ S)) & 1;
	uint32_t J2 = (~(I2 ^ S)) & 1;

	uint32_t opc = 0;

	// Fixed bits
	opc |= 0b11110u << 27;   // bits 31:27
	opc |= S << 26;         // bit 26
	opc |= imm10 << 16;     // bits 25:16
	opc |= 0b10u << 14;     // bits 15:14
	opc |= J1 << 13;        // bit 13
	opc |= 1u << 12;        // bit 12 (always 1)
	opc |= J2 << 11;        // bit 11
	opc |= imm11;           // bits 10:0

	return arrange_thumb_4byte(opc);
}

uint16_t emit_bx_t1(uint8_t rm) {
	uint16_t opc = 0;

	opc |= 0b010001110 << 7;
	opc |= rm << 3;
	// opc |= 0b000 << 0;

	return opc;
}

#endif //__arm__
