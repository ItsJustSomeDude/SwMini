#ifndef SWMINI_PROBER_H
#define SWMINI_PROBER_H

#include <stdbool.h>

typedef struct {
    unsigned long start;
    unsigned long end;

    bool readable;
    bool writable;
    bool executable;

    bool private;
    bool shared;

    const char* path;
} ValidPage;

void updateAreas();

bool readablePointer(const char* tag, void* ptr);

void probePointer(const char* tag, void* ptr);

#endif //SWMINI_PROBER_H
