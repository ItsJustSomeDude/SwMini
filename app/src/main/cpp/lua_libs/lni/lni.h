#ifndef SWMINI_LUA_LNI_H
#define SWMINI_LUA_LNI_H

#include "lualib.h"

#include <jni.h>

// See the LuaNativeInterface::getLNIType method.
#define LNI_TYPE_VOID 0
#define LNI_TYPE_BOOL 1
#define LNI_TYPE_NUM 2
#define LNI_TYPE_STR 3

typedef struct {
	jclass clazz;       // Global Reference to jclass owning this method
	jmethodID mid;      // Pointer to method ID
	int *params;        // Pointer to dynamic array of integers
	size_t paramsLength;// Size of params array

	int returnType;     // LNI Return Type of function

	// We don't need the signature, or the name, because the name will be the hash table key,
	// And since we have the methodID we don't need the signature.
} LNIMethod;

LNIMethod *get_lni_method(const char *methodName);

int lni_execute(lua_State *L);
int lni_bind(lua_State *L);

void lni_bind_global(lua_State *L, const char *func, int count, ...);

#endif //SWMINI_LUA_LNI_H
