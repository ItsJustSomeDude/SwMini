#ifdef __arm__

#ifndef NEEDLE_THUMB_H
#define NEEDLE_THUMB_H

#include <stdint.h>

uint32_t emit_movw_t3(uint8_t rd, uint16_t imm16);

#endif //NEEDLE_THUMB_H
#endif //__arm__
