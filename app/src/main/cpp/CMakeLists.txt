cmake_minimum_required(VERSION 3.4.1)

project("SwMini Natives")

add_library(mini SHARED
        lua_ext/mini_files.c
        lua/ltablib.c
        lua/lmathlib.c
        lua/ldblib.c
        lua/loslib.c
        lua/liolib.c

#        prober.c
#        network.c

        caver.c
        lua_core.c
        lua_libraries.c
        main.c
        hook_core.c
        hook.c
        patches/cstrings.c
        lni.c

        jni.c

        lni2.c

        patches/32patch.c

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

        lua_ext/lfs.c
)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")

# Add prebuilt static library
add_library(GlossHook SHARED IMPORTED)

# Specify the path to the .so file for each architecture
set_target_properties(
        GlossHook
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libGlossHook.so
)

target_include_directories(mini PUBLIC ${CMAKE_SOURCE_DIR}/lua)

find_library(log-lib log)

target_link_libraries(
    mini
    GlossHook
    ${log-lib}
)
