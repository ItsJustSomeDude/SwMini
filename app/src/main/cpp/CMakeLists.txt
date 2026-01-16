cmake_minimum_required(VERSION 3.4.1)

project("SwMini Natives")

add_library(mini SHARED
        assembly/thumb.c
        assembly/arm64.c

        lua/ltablib.c
        lua/lmathlib.c
        lua/ldblib.c
        lua/loslib.c
        lua/liolib.c

        lua_libs/lni/register.c
        lua_libs/lni/execute.c
        lua_libs/lni/bind.c

        lua_libs/mini/controls.c
        lua_libs/mini/library.c
        lua_libs/mini/profile.c
        lua_libs/mini/arch.c
        lua_libs/mini/scene_find_all.c
        lua_libs/mini/debug.c
        lua_libs/mini/recreate_hero.c

        # prober.c
        # network.c

        lua_core.c
        main.c
        hooks.c
        #        lni.c

        log.c

        jni.c
        java.c

        patches/32patch.c
        patches/assets.c
        patches/panic.c
        patches/lua_libs.c

        luasocket/src/luasocket.c
        luasocket/src/io.c
        luasocket/src/select.c
        luasocket/src/auxiliar.c
        luasocket/src/buffer.c
        luasocket/src/compat.c
        luasocket/src/usocket.c
        luasocket/src/except.c
        luasocket/src/timeout.c
        luasocket/src/inet.c
        luasocket/src/tcp.c
        luasocket/src/udp.c
        luasocket/src/options.c

        lua_libs/lfs.c

        features/coin_limit.c
        features/lua_paths.c

        features/cstrings/workarounds.c
        features/cstrings/register.c
        features/cstrings/hooks.c

        #        stdlib/gabi++/include/new
        #        stdlib/stdlib/new
        iterate.cpp
)

# Add prebuilt static library
add_library(GlossHook SHARED IMPORTED)

# Specify the path to the .so file for each architecture
set_target_properties(
        GlossHook
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libGlossHook.so
)

target_include_directories(mini
        PUBLIC ${CMAKE_SOURCE_DIR}/lua
        #        PUBLIC ${CMAKE_SOURCE_DIR}/stdlib/stdlib
        #        PUBLIC ${CMAKE_SOURCE_DIR}/stdlib/stdlib/stl
        #        PUBLIC ${CMAKE_SOURCE_DIR}/gabi++/include
)
#target_include_directories(mini PUBLIC ${CMAKE_SOURCE_DIR}/stdlib)

#target_include_directories(mini SYSTEM PUBLIC ${CMAKE_SOURCE_DIR}/stdlib)

#include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/stdlib)

find_library(log-lib log)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DNDK_DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
endif ()

target_link_libraries(
        mini
        GlossHook
        ${log-lib}
)
