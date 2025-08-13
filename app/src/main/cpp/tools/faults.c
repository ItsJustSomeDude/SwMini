#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#ifdef ENABLE_FAULT_INJECTION
#include "faults.c"
#define malloc(size) my_malloc(size)
#define realloc(buffer, size) my_realloc(buffer, size)
#endif

#include "../lni.h"

void send_lni_buffer(const char *buffer) {
//	printf("=> LNI:%s <=\n", buffer);
}

void send_log_buffer(const char *buffer) {
}

static int malloc_fail_counter = 0;
static int malloc_fail_every = 200; // 0 = no failure, otherwise fail every Nth malloc

void* my_malloc(size_t size) {
    if (malloc_fail_every > 0) {
        malloc_fail_counter++;
        if (malloc_fail_counter % malloc_fail_every == 0) {
            // Simulate malloc failure
            return NULL;
        }
    }
    return malloc(size);
}

void* my_realloc(char *buffer, size_t size) {
    if (malloc_fail_every > 0) {
        malloc_fail_counter++;
        if (malloc_fail_counter % malloc_fail_every == 0) {
            // Simulate malloc failure
            return NULL;
        }
    }
    return realloc(buffer, size);
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    int fds[2];
    if (pipe(fds) != 0) return 0;  // failed to create pipe

    // Write fuzzer input into the pipe
    write(fds[1], data, size);
    close(fds[1]);  // Important: close write end after writing

    // Call your function with the read end
    process_lni_pipe(fds[0]);
    close(fds[0]);  // cleanup
    return 0;
}

