
#ifndef NEEDLE_COIN_LIMIT_H
#define NEEDLE_COIN_LIMIT_H

void initF_coin_limit();

void miniCL_set_default(unsigned short limit);
void miniCL_set(unsigned short limit);
void miniCL_set_too_rich(unsigned short amount);
void miniCL_reset();

#endif //NEEDLE_COIN_LIMIT_H
