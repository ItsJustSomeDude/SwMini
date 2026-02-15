#ifndef NEEDLE_ACHIEVEMENTS_H
#define NEEDLE_ACHIEVEMENTS_H

#include "features/cstrings/cstrings.h"
#include "hooks.h"

// All we really need is the size + padding, but this should be the actual structure layout.
typedef struct {
	CppString *id;
	CppString *name;
	CppString *description;
	int thing1;
	CppString *counter;
	int thing2;
} Achievement;

// This Header is shared between 32 and 64bit. The actual instance is initialized in either file.
H_DL_FUNCTION(
	achievement_init,
	void,
	(Achievement * this, CppString * *id, CppString * *name,
		CppString * *description, int points, CppString **counter, int counter_required)
)

void create_achievement(
	const char *id, const char *name,
	const char *desc, int points,
	const char *counter, int threshold
);

void miniACH_register();

void init_feature_achievements();

#endif //NEEDLE_ACHIEVEMENTS_H
