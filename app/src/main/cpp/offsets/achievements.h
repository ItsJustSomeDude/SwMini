#ifndef NEEDLE_ACHIEVEMENTS_OFFSETS_H
#define NEEDLE_ACHIEVEMENTS_OFFSETS_H

#include "base.h"

/* This is the body of the `while` loop in the constructor method.
 * Branch to here. */
#define MOFF_AchievementsManagerInit_CreateStart offset_(0xf9e, 0x16dc)

/* This is the instruction following the end of the loop body.
 * A "Return" instruction will be written here. must */
#define MOFF_AchievementsManagerInit_CreateEnd offset_(0x1112, 0x1a70)

/* This is the start of the Vanilla Achievement initialization, to be skipped. */
#define MOFF_AchievementsManagerInit_VanillaSkipStart offset_(0x252, 0xf4)

/* This is the branch target after the Vanilla Achievement initialization.
 * It's after the loop, right before the stack canary check. */
#define MOFF_AchievementsManagerInit_VanillaSkipEnd offset_(0x1154, 0x1c14)

#endif //NEEDLE_ACHIEVEMENTS_OFFSETS_H
