#include "prober.h"

#include "log.h"

#include <linux/limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TAG "MiniProber"

static ValidPage *areas = NULL;
static long areaLen = 0;

bool readablePointer(const char *tag, void *ptr) {
	if (ptr == NULL) return false;

	LOGD("Checking pointer %p", ptr);

	unsigned long p = (unsigned long) ptr;

	LOGD("Beginning loop...");
	for (long i = 0; i < areaLen - 1; i++) {
		ValidPage page = areas[i];
//        LOGD("Checking %lu against %lu -> %lu (%i)", (unsigned long)ptr, page.start, page.end, page.readable);
//        LOGD("??? %i", (unsigned long)ptr < page.end);
//        LOGD("Tests... %i", (unsigned long)ptr >= page.start, (unsigned long)ptr < page.end, page.readable);
//        LOGD("Tests... %i", (unsigned long)ptr < page.end);
//        LOGD("Tests... %i", page.readable);

		if ((unsigned long) ptr > page.start && (unsigned long) ptr < page.end && page.readable) {
			LOGD("%s: Valid, pointer falls in somewhere", tag);
			return true;
		}
	}

	LOGD("Not readable");
	return false;
}

void updateAreas() {
	LOGD("Starting Area Scan");

	FILE *maps = fopen("/proc/self/maps", "r");
	if (!maps) {
		perror("Failed to open /proc/self/maps");
		return;
	}

	// Count lines.
	long count = 0;
	int c;
	while ((c = getc(maps)) != EOF) {
		if (c == '\n') {
			count++;
		}
	}

	// Reset the file pointer to the beginning
	fseek(maps, 0, SEEK_SET);

	if (areas != NULL) {
		// TODO: Free all the paths, they leak right now.
		free(areas);
	}
	areas = malloc(count * sizeof(ValidPage));
	if (areas == NULL) {
		perror("OOM");
		return;
	}
	areaLen = count;

	unsigned long start, end;
	char permissions[5];
	char line[256];

	int i = 0;
	while (fgets(line, sizeof(line), maps)) {
		// Parse the address range and permissions
		char path[PATH_MAX] = "";

		if (sscanf(line, "%lx-%lx %4s %*x %*s %*s %s", &start, &end, permissions, path) != 4) {
			continue;
		}

		areas[i].start = start;
		areas[i].end = end;

		areas[i].readable = permissions[0] == 'r';
		areas[i].writable = permissions[1] == 'w';
		areas[i].executable = permissions[2] == 'x';
		areas[i].private = permissions[3] == 'p';
		areas[i].shared = permissions[3] == 's';

		areas[i].path = strdup(path);

//        LOGD("Uh... lib: %s, start: %lu, end: %lu, perms: [%s]", areas[i].path, start, end, permissions);
	}

	fclose(maps);
}

void probePointer(const char *tag, void *ptr) {
	if (ptr == NULL) {
		LOGI("%s: Found Null Pointer.", tag);
		return;
	}

	if (!readablePointer(tag, ptr)) {
		LOGI("%s: Pointer is unreadable.", tag);
		return;
	}

	LOGI("Attempting deref...");
	void *deref = *(void **) ptr;

	if (deref == NULL) {
		LOGI("%s: Data at target is NULL.", tag);
		return;
	}

	if (readablePointer(tag, ptr)) {
		LOGI("%s: Appears to be pointer to pointer - %p? Beginning sub-probe...", tag, ptr);
		probePointer(tag, deref);
		return;
	}

	LOGI("%s: Hit the end.", tag);
}
