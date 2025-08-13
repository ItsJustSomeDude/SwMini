
#ifndef SWMINI_LNI_H
#define SWMINI_LNI_H

// Function prototypes for the handlers
void send_log_buffer(const char *buffer);
void send_lni_buffer(char *buffer);
void *lni_thread(void *_);

// Buffer size constants
#define LOG_BUFFER_SIZE 128
#define INITIAL_LNI_SIZE 128
// \x75 == \127 in Lua.
#define START_MARKER "\x7FLNI:"
#define END_MARKER ");"

/**
 * Reads from a pipe and processes data according to markers.
 * Normal data is processed in chunks for logging.
 * LNI chunks are slurped and passed over JNI.
 *
 * @param read_fd The file descriptor to read from
 * @return 0 on success, non-zero on error
 */
int process_lni_pipe(void);

int spawn_lni_thread(void);

#endif //SWMINI_LNI_H
