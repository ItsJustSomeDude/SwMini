
#ifndef NEEDLE_COIN_LIMIT_H
#define NEEDLE_COIN_LIMIT_H

/** This is set to the Vanilla default (999) in the init_feature function.
 * It should be set from Java to the mini.properties value. */
extern unsigned short default_coin_limit;
extern unsigned short current_coin_limit;

extern unsigned short current_too_rich;

void init_feature_coin_limit();

void set_coin_limit(unsigned short limit);
void reset_coin_limit();
void set_too_rich_amount(unsigned short amount);

#endif //NEEDLE_COIN_LIMIT_H
