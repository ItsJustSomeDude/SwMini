#ifdef __arm__

#include "hooks.h"
#include "log.h"
#include "patches.h"

#define LOG_TAG "Mini32Patch"

typedef struct {
	void *something;
	int cachedSize;
	const char **name;
	const char **string;
	const char **bytes;
	int presenceFlags;
} ProtoProgram;

static const char **default_proto_string;

STATIC_DL_HOOK_SYMBOL(
	codedStream,
	"_ZN5Caver5Proto7Program27MergePartialFromCodedStreamEPN6google8protobuf2io16CodedInputStreamE",
	long, (ProtoProgram * this, void *stream)
) {
//	LOGD("32bit ::Proto::Program::MergePartial call begin.");
//	LOGD("Args: this = %p, stream = %p", this, stream);

//	LOGD("Starter Values in Stream: ", iter, field8, field14);

	long ret = orig_codedStream(this, stream);

	bool namePresent = this->presenceFlags & 0b001;
	bool stringPresent = this->presenceFlags & 0b010;
	bool bytesPresent = this->presenceFlags & 0b100;

	if (!stringPresent) {
		// Empty/default program, or maybe Bytes is present and will be used.
		return ret;
	}

	// The string block is empty. Maybe there's a bytes block, just fall through again.
	if (**this->string == '\0') {
		LOGD("Initializing a Program with an empty string block in the proto.");
		return ret;
	}

//	LOGD("Presents? %b %b %b", namePresent, stringPresent, bytesPresent);
//	const char *stringContent = *this->string;
//	LOGD("The string block is: (%p) %.10s...", stringContent, stringContent);

	const char **origBytes = this->bytes;
	const char **origString = this->string;

	// If there was a bytes block set, we must copy it to the string block so it will be freed properly by the engine.
	if (bytesPresent) {
		// Bytes is present, String is present.
		// We must leave Bytes in String, so the engine will free it when ready.
		this->string = origBytes;
	} else {
		// Bytes is absent, String is present.
		// Remove the string by replacing it with the Default, and clearing it's flag.
		this->string = default_proto_string;
		stringPresent = false;
	}

	// Bytes is ready to receive, String is present,
	this->bytes = origString;
	bytesPresent = true;

	// Rebuild the flags.
	int rebuilt = (namePresent & 1) | ((stringPresent & 1) << 1) | ((bytesPresent & 1) << 2);
	// LOGD("Old: %i; New: %i", this->presenceFlags, rebuilt);

	this->presenceFlags = rebuilt;

	LOGD("Successfully patched Program at %p to execute the String block.", this);

	return ret;
}

void init_patch_32bit() {
	LOGD("Applying 32bit Program::Bytes patch");

	default_proto_string = offset_address(0x464710);

	hook_codedStream();
}

#endif //__arm__
