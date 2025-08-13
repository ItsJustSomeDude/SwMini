#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "lni.h"

static int stdout_pipe[2];
static pthread_t loggingThread;

int process_lni_pipe() {
    int read_fd = stdout_pipe[0];

	bool reading_lni = false;

	char *lni_buffer = NULL;
	size_t lni_size = 0;
	size_t lni_pos = 0;

    // START marker detection state
    int start_match_pos = 0;
    // END marker detection state
    int end_match_pos = 0;

    char log_buffer[LOG_BUFFER_SIZE];
    size_t log_pos = 0;

	char current_byte;
    ssize_t bytes_read;

    while ((bytes_read = read(read_fd, &current_byte, 1)) > 0) {
		// Always send to log buffer

        // Don't send the LNI Special char to the actual log.
        if(current_byte == START_MARKER[0]) {
            log_buffer[log_pos++] = '+';
        } else if (current_byte == '\n') {
            // Skip advancing, then flush, if a newline is hit.
            // log_pos--;
        } else {
            log_buffer[log_pos++] = current_byte;
        }

		// If log buffer is full, or ends with newline, flush it.
		if (
			(current_byte == '\n'/* && log_pos > 1*/) ||
			log_pos == LOG_BUFFER_SIZE - 1
		) {
			log_buffer[log_pos] = '\0';
			send_log_buffer(log_buffer);
			log_pos = 0;
		}

		// Hello, world! LNI:print("huh");\n
		// Final LNI does not contain LNI: but does contain );\n

		if (reading_lni) {
			// Grow buffer if it's too smol
			// +1 for current byte, +1 for \0 terminate
			if (lni_pos + 2 >= lni_size) {
				lni_size = lni_size * 2 + 1;
				char* new_buffer = realloc(lni_buffer, lni_size);
				if (!new_buffer) {
					// Buffer Grow Failed.
					free(lni_buffer);
					lni_buffer = NULL;
					lni_size = 0;
					lni_pos = 0;
					end_match_pos = 0;
					reading_lni = false;
					continue;
				}
				lni_buffer = new_buffer;
			}

            // This shouldn't ever run, but it makes clang happy.
            if (lni_buffer == NULL) continue;

			lni_buffer[lni_pos++] = current_byte;

        	if (current_byte == END_MARKER[end_match_pos]) {
            	if (end_match_pos == strlen(END_MARKER) - 1) {
					// Reached end of end marker.
					// The whole end marker is already in the buffer too.

					// There should always be room for the null byte thanks to the Grow code.
					lni_buffer[lni_pos] = '\0';

					// Do the JNI call now.
					send_lni_buffer(lni_buffer);

					reading_lni = false;
					free(lni_buffer);
					lni_buffer = NULL;
					lni_pos = 0;
					lni_size = 0;
					end_match_pos = 0;
				} else {
					// Reached next of marker.
					end_match_pos++;
				}
			} else if (current_byte == '\n') {
				// We hit the end of line without the rest of the end marker.
				// Free buffer and bail out.
				reading_lni = false;
				free(lni_buffer);
				lni_buffer = NULL;
				lni_pos = 0;
				lni_size = 0;
				end_match_pos = 0;
			} else if (end_match_pos > 0) {
				// Non-end char, reset ending state.
				end_match_pos = 0;
			}
		} else if (current_byte == START_MARKER[start_match_pos]) {
			// Not reading LNI, found the next Start Marker char.
			if (start_match_pos == strlen(START_MARKER) - 1) {
				// Matched the last start char!
				start_match_pos = 0;

				lni_buffer = malloc(INITIAL_LNI_SIZE);
				if (lni_buffer) {
					reading_lni = true;
					lni_size = INITIAL_LNI_SIZE;
				} else {
					// no memory!
				}
			}
			start_match_pos++;
		} else if (start_match_pos > 0) {
			// Read part of the start marker, but now theres a non-match.
			start_match_pos = 0;
		}
    }

	// Pipe closed.
	// Log any data remaining in the log buffer.
	if(log_pos > 0) {
        log_buffer[log_pos] = '\0';
		send_log_buffer(log_buffer);
	}

	// Drop any leftover data from LNI.
	if (lni_buffer) free(lni_buffer);

    if (bytes_read < 0) {
        return -1;
    }

    return 0;
}

int spawn_lni_thread()
{
    /* make stdout line-buffered and stderr unbuffered */
    setvbuf(stdout, 0, _IOLBF, 0);
    setvbuf(stderr, 0, _IONBF, 0);

    /* create the pipe and redirect stdout and stderr */
    pipe(stdout_pipe);
    dup2(stdout_pipe[1], 1);
    dup2(stdout_pipe[1], 2);

    /* spawn the logging thread */
    pthread_create(&loggingThread, 0, lni_thread, 0);
    pthread_detach(loggingThread);
    return 0;
}

