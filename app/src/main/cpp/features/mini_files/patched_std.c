#include "patched_std.h"
#include "functions.h"

/* This is complicated and annoying, because there is _technically_ no guarantee that stdin/out/err do not move. */

#undef stdin
#undef stdout
#undef stderr

MiniFILE stdin__ = {REAL, NULL};
MiniFILE stdout__ = {REAL, NULL};
MiniFILE stderr__ = {REAL, NULL};

MiniFILE *stdin_() {
	stdin__.file = stdin;
	return &stdin__;
}

MiniFILE *stdout_() {
	stdout__.file = stdout;
	return &stdout__;
}

MiniFILE *stderr_() {
	stderr__.file = stderr;
	return &stderr__;
}
