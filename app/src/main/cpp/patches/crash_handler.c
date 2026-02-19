#include "log.h"

#include <asm/fcntl.h>
#include <asm/signal.h>
#include <bits/signal_types.h>
#include <fcntl.h>
#include <jni.h>
#include <signal.h>
#include <stddef.h>
#include <unistd.h>

#define LOG_TAG "MiniCrashHandler"

static int crash_marker_fd = -1;

static void crash_handler(int sig, siginfo_t *info, void *ctx) {
	if (crash_marker_fd >= 0) {
		const char marker[] = "crash\n";
		write(crash_marker_fd, marker, sizeof(marker) - 1);
		close(crash_marker_fd);   // optional but fine
	}

	signal(sig, SIG_DFL);   // restore default
	raise(sig);             // rethrow so normal crash handling happens

	/* This code is not guaranteed to be async safe, so it's "best-effort" beyond here. */

	LOGE("Crash marker written. Logs will be captured on next startup!");
}

static void install_crash_handlers() {
	struct sigaction sa;
	sa.sa_sigaction = crash_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;

	sigaction(SIGSEGV, &sa, NULL);
	sigaction(SIGABRT, &sa, NULL);
	sigaction(SIGBUS, &sa, NULL);
	sigaction(SIGILL, &sa, NULL);
}

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_CrashHandler_setupNativeCrashHandler(
	JNIEnv *env, jclass clazz, jstring jPath
) {
	const char *path = (*env)->GetStringUTFChars(env, jPath, 0);

	LOGD("Enabling crash handler, marker is %s", path);

	// Open a file in the Cache
	crash_marker_fd = open(
		path,
		O_WRONLY | O_CREAT | O_TRUNC | O_DSYNC,
		0644
	);

	(*env)->ReleaseStringUTFChars(env, jPath, path);

	install_crash_handlers();
}

