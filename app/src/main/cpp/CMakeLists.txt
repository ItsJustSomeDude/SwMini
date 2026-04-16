cmake_minimum_required(VERSION 3.4.1)

project("SwMini Natives")

find_program(IWYU_EXECUTABLE NAMES include-what-you-use iwyu)
if(IWYU_EXECUTABLE)
    set(CMAKE_C_INCLUDE_WHAT_YOU_USE
        "${IWYU_EXECUTABLE};-Xiwyu;--ignore_re=/lua/"
    )
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE
        "${IWYU_EXECUTABLE};-Xiwyu;--ignore_re=/lua/"
    )
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
else()
    message(WARNING "IWYU executable not found, header dependencies will not be checked.")
endif()

add_library(mini SHARED
        main.c

        assembly/thumb.c
        assembly/arm64.c

        core/assets.c
        core/cpp_strings.c
        core/hooks.c
        core/log.c
        core/lua.c
        core/jni.c
        core/java.c
        core/library_preload.c

        "core/impl_files/paths.c"
        core/impl_files/path_parser.c
        core/impl_files/resource_paths.c

        core/impl_files/backend_assets/backend.c
        core/impl_files/backend_assets/dirent.c
        core/impl_files/backend_assets/stat.c
        core/impl_files/backend_assets/stdio.c
        core/impl_files/backend_assets/stdlib.c
        core/impl_files/backend_assets/unistd.c
        core/impl_files/backend_assets/utime.c

        core/impl_files/backend_std/backend.c
        core/impl_files/backend_std/dirent.c
        core/impl_files/backend_std/stat.c
        core/impl_files/backend_std/stdio.c
        core/impl_files/backend_std/stdlib.c
        core/impl_files/backend_std/unistd.c
        core/impl_files/backend_std/utime.c

        core/files/dirent.c
        core/files/stat.c
        core/files/stdio.c
        core/files/stdio_files.c
        core/files/stdlib.c
        core/files/unistd.c
        core/files/utime.c

        caver/components.c
        caver/program_state.c

        config/config.c
        config/mini_config.c

        events/profile.c

        utils/byte_sequence.c
        utils/files.c

        lua/ldblib.c
        lua/linit.c
        lua/liolib.c
        lua/lmathlib.c
        lua/loadlib.c
        lua/loslib.c
        lua/ltablib.c

        lua_libs/lfs/lfs.c

        lua_libs/lni/register.c
        lua_libs/lni/execute.c
        lua_libs/lni/lni.c

        lua_libs/mini/controls.c
        lua_libs/mini/library.c
        lua_libs/mini/profile.c
        lua_libs/mini/arch.c
        lua_libs/mini/scene_find_all.c
        lua_libs/mini/debug.c
        lua_libs/mini/recreate_hero.c
        lua_libs/mini/the_map_function.c

        lua_libs/mini_health/health.c

        lua_libs/mini_character/mini_character.c
        lua_libs/mini_character/attributes.c
        lua_libs/mini_character/exp.c

        lua_libs/components/library.c
        lua_libs/components/components.c
        lua_libs/components/health.c
        lua_libs/components/physics.c
        lua_libs/components/entity.c
        lua_libs/components/swingable_weapon_controller.c

        patches/32patch.c
        patches/assets.c
        patches/panic.c
        patches/crash_handler.c
        patches/lua_load_file.c
        patches/program_state.c

        features/achievements/achievements.c
        features/achievements/ach64.c
        features/achievements/ach32.c

        features/coin_limit.c
        features/armor_models.c

        features/cstrings/workarounds.c
        features/cstrings/register.c
        features/cstrings/hooks.c
        features/cstrings/file.c

        # 3rd-party libs (Git Submodules)

        libs/luasocket/src/luasocket.c
        libs/luasocket/src/io.c
        libs/luasocket/src/select.c
        libs/luasocket/src/auxiliar.c
        libs/luasocket/src/buffer.c
        libs/luasocket/src/compat.c
        libs/luasocket/src/usocket.c
        libs/luasocket/src/except.c
        libs/luasocket/src/timeout.c
        libs/luasocket/src/inet.c
        libs/luasocket/src/tcp.c
        libs/luasocket/src/udp.c
        libs/luasocket/src/options.c

        libs/toml/toml.c

        # C++ Experiments (blank for now)
)

# GlossHook - prebuilt static library
add_library(GlossHook SHARED IMPORTED)
set_target_properties(
        GlossHook
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libGlossHook.so
)

# Base include directories
target_include_directories(mini
        PUBLIC ${CMAKE_SOURCE_DIR}
        PUBLIC ${CMAKE_SOURCE_DIR}/lua
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DNDK_DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
endif ()

target_link_libraries(mini
        GlossHook
        log
        android
)
