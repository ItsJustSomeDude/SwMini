#include <utime.h>
#include "backend.h"
#include "core/files/utime.h"

int std_utime(const char *_Nonnull filename, const mini_utimbuf *_Nullable times) {
	struct utimbuf buf = {
		.actime = times->actime,
		.modtime = times->modtime
	};
	
	return utime(filename, &buf);
}
