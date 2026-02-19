#include "iterate.h"

#include "map"

#include <stdio.h>

//template<typename A, typename B>
void iterate_map(void *mapPointer /*, void (*exec)(A key, B value) */) {
	auto *map = reinterpret_cast<std::map<void *, void *> *>(mapPointer);

//	printf("Begin? %p", map.begin());

//	printf("Begin: %p End: %p", map.begin, map.end);

	for (auto it = map->begin(); it != map->end(); ++it) {
		printf("Loop thing hit");
		printf("Loop hit. %p %p", it->first, it->second);
	}



//	auto *castedMapPointer =
//
//	std::map<std::string *, void *> m2 = reinterpret_cast<std::map<std::string, void *>>(mapPointer);
//
}
