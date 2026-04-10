#ifndef NEEDLE_BASE_OFFSETS_H
#define NEEDLE_BASE_OFFSETS_H

#if defined(__arm__)
#define offset_(o32, o64) o32
#elif defined(__aarch64__)
#define offset_(o32, o64) o64
#endif

#endif //NEEDLE_BASE_OFFSETS_H
