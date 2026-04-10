#include "stdio_files.h"
#include "core/impl_files/backend_std/backend.h"
#include "core/impl_files/mini_file.h"
#include <stdio.h>

/* Mini-compatible std-streams. The usable names are defined in the header. */

MiniFILE stdin__ = {.backend = NULL, .std_file = NULL};
MiniFILE stdout__ = {.backend = NULL, .std_file = NULL};
MiniFILE stderr__ = {.backend = NULL, .std_file = NULL};

MiniFILE *miniF_stdin_() {
	stdin__.backend = &backend_std;
	stdin__.std_file = stdin;
	return &stdin__;
}

MiniFILE *miniF_stdout_() {
	stdout__.backend = &backend_std;
	stdout__.std_file = stdout;
	return &stdout__;
}

MiniFILE *miniF_stderr_() {
	stderr__.backend = &backend_std;
	stderr__.std_file = stderr;
	return &stderr__;
}
