#include <stdio.h>
#include <string.h>
#include <android/log.h>
#include <jni.h>
#include <dlfcn.h>
#include <math.h>


#include "log.h"
#include "hook_core.h"
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "libs/Gloss.h"
#include "symbol.h"


#define LOG_TAG "MiniHook"


void setupHooks() {
}

