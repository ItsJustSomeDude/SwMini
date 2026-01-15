#ifdef __arm__

#include <stdint.h>
#include "thumb.h"
#include "utils.h"

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


#endif //__arm__
