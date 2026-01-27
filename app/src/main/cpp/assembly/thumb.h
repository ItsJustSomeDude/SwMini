#ifdef __arm__

#ifndef NEEDLE_THUMB_H
#define NEEDLE_THUMB_H

#include <stdint.h>

uint32_t emit_movw_t3(uint8_t rd, uint16_t imm16);
uint16_t emit_b_t2(int16_t target);
uint32_t emit_b_t4(int32_t target);
uint16_t emit_bx_t1(uint8_t rm);

#endif //NEEDLE_THUMB_H
#endif //__arm__
