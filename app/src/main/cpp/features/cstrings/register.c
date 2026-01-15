#include <stddef.h>
#include <string.h>
#include <jni.h>
#include "cstrings.h"
#include "../../libs/khash.h"
#include "../../log.h"

#define LOG_TAG "MiniCStringRegister"

KHASH_MAP_INIT_STR(str, char*)

static khint_t k;
static kh_str_t *replacements_table = NULL;

void cstrings_add_replacement(const char *source, const char *replacement) {
	if (source == NULL || *source == '\0') return;

	// Create the table if this is the first replacement
	if (replacements_table == NULL)
		replacements_table = kh_init_str();

	// If we have a workaround for the given Source string, we must skip the regular registration.
	bool workaround_applied = cstrings_apply_workarounds(source, replacement);
	if (workaround_applied) return;

	const char *ourSource = strdup(source);

	int absent;
	k = kh_put_str(replacements_table, ourSource, &absent);
	if (absent) {
		LOGD("Added new string: '%s' -> '%s'", ourSource, replacement);
		kh_value(replacements_table, k) = strdup(replacement);
	}
}

const char *cstrings_get_replacement(const char *source) {
	if (replacements_table == NULL) return NULL;

	k = kh_get_str(replacements_table, source);
	if (k == kh_end(replacements_table)) return NULL;

	const char *v = kh_value(replacements_table, k);
	return v;
}

// TODO: I don't think this actually frees the values?
void destroy_feature_cstrings() {
	if (replacements_table == NULL) return;

	// IMPORTANT: free memory allocated by strdup() above
	for (k = 0; k < kh_end(replacements_table); ++k)
		if (kh_exist(replacements_table, k))
			free((char *) kh_key(replacements_table, k));
	kh_destroy_str(replacements_table);
}

JNIEXPORT void JNICALL
Java_net_itsjustsomedude_swrdg_NativeBridge_addStringReplacement(
	JNIEnv *env, jclass clazz, jstring jSource, jstring jReplacement
) {
	const char *source = (*env)->GetStringUTFChars(env, jSource, 0);
	const char *replacement = (*env)->GetStringUTFChars(env, jReplacement, 0);

	cstrings_add_replacement(source, replacement);

	(*env)->ReleaseStringUTFChars(env, jSource, source);
	(*env)->ReleaseStringUTFChars(env, jReplacement, replacement);
}
