#include "log.h"

#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define LOG_TAG "LogThreads"

void pipe_handler(const int pipe[2], void (*send_log)(const char *buffer)) {
	int read_fd = pipe[0];
	char log_buffer[LOG_BUFFER_SIZE];
	size_t log_pos = 0;

	char current_byte;
	ssize_t bytes_read;

	while ((bytes_read = read(read_fd, &current_byte, 1)) > 0) {
		if (current_byte == '\n' || log_pos == LOG_BUFFER_SIZE - 1) {
			// If log buffer is full, or ends with newline, flush it.
			log_buffer[log_pos] = '\0';
			send_log(log_buffer);
			log_pos = 0;
		} else {
			// Add character to log buffer
			log_buffer[log_pos++] = current_byte;
		}
	}

	// Pipe closed. Log any data remaining in the log buffer.
	if (log_pos > 0) {
		log_buffer[log_pos] = '\0';
		send_log(log_buffer);
	}
}

void send_log_buffer(const char *buffer) {
	__android_log_write(ANDROID_LOG_DEBUG, "libswordigo", buffer);
}

void *start_stdout_thread(void *_handle) {
	/* Make stdout line-buffered */
	setvbuf(stdout, 0, _IOLBF, 0);

	/* Create Pipe, attach to stdout */
	int stdout_pipe[2];
	pipe(stdout_pipe);
	dup2(stdout_pipe[1], STDOUT_FILENO);

	LOGD("Starting stdout reader...");

	/* Read and consume from pipe. */
	pipe_handler(stdout_pipe, send_log_buffer);

	return NULL;
}

void send_error_buffer(const char *buffer) {
	__android_log_write(ANDROID_LOG_ERROR, "libswordigo", buffer);
}

void *start_stderr_thread(void *_handle) {
	/* Make stderr unbuffered */
	setvbuf(stderr, 0, _IOLBF, 0);

	/* Create Pipe, attach to stdout */
	int stderr_pipe[2];
	pipe(stderr_pipe);
	dup2(stderr_pipe[1], STDERR_FILENO);

	LOGD("Starting stderr reader...");

	/* Read and consume from pipe. */
	pipe_handler(stderr_pipe, send_error_buffer);

	return NULL;
}

static pthread_t stdout_thread;
static pthread_t stderr_thread;

int spawn_log_threads() {
	pthread_create(&stdout_thread, 0, start_stdout_thread, 0);
	pthread_detach(stdout_thread);

	pthread_create(&stderr_thread, 0, start_stderr_thread, 0);
	pthread_detach(stderr_thread);

	return 0;
}
