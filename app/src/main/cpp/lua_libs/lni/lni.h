#ifndef SWMINI_LUA_LNI_H
#define SWMINI_LUA_LNI_H

#include "lualib.h"

#include <jni.h>

// See the LuaNativeInterface::getLNIType method.
typedef enum LNIType {
	LNI_TYPE_VOID = 0,
	LNI_TYPE_BOOL = 1,
	LNI_TYPE_NUM = 2,
	LNI_TYPE_STR = 3,
} LNIType;

typedef struct {
	jclass clazz;           // Global Reference to jclass owning this method
	jmethodID mid;          // Pointer to method ID
	LNIType *params;        // Pointer to dynamic array of integers
	size_t params_count;    // Size of params array

	LNIType returnType;     // LNI Return Type of function

	// We don't need the signature, or the name, because the name will be the hash table key,
	// And since we have the methodID we don't need the signature.
} LNIMethod;

LNIMethod *lni_get_method(const char *methodName);

int lni_exec_name(lua_State *L, const char *method_name);

int miniLL_lni_execute(lua_State *L);
int miniLL_lni_bind(lua_State *L);

#endif //SWMINI_LUA_LNI_H
