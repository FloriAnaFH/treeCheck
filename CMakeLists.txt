cmake_minimum_required(VERSION 3.15)
project(treecheck LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to Release for single-config generators (e.g. Make, Ninja)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

# ── Platform-specific compiler options ────────────────────────────────────────
if(MSVC)
    # /W4       – high warning level
    # /utf-8    – treat source and execution charset as UTF-8
    # NOMINMAX  – prevent windows.h min/max macros clashing with std::min/max
    add_compile_options(/W4 /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ── Core library (no main) ────────────────────────────────────────────────────
add_library(treecheck_lib STATIC
    src/tree.cpp
    src/stats.cpp
    src/modes.cpp
)

# Expose headers to every target that links treecheck_lib
target_include_directories(treecheck_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# GCC < 9 and some MinGW toolchains require an explicit link to stdc++fs
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND
   CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(treecheck_lib PUBLIC stdc++fs)
endif()

# ── Main executable ───────────────────────────────────────────────────────────
add_executable(treecheck
    main.cpp
)
target_link_libraries(treecheck PRIVATE treecheck_lib)

# When cross-compiling for Windows with MinGW, statically link all MinGW
# runtime libraries (libgcc, libstdc++, libwinpthread) so the resulting .exe
# runs without any MinGW DLLs on the target machine.
if(MINGW)
    target_link_options(treecheck PRIVATE -static)
endif()

# ── Tests ─────────────────────────────────────────────────────────────────────
enable_testing()

add_executable(treecheck_tests
    tests/test_all.cpp
)
target_link_libraries(treecheck_tests PRIVATE treecheck_lib)
target_compile_definitions(treecheck_tests PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests"
)

add_test(NAME treecheck_tests COMMAND treecheck_tests)

# ── Install ───────────────────────────────────────────────────────────────────
include(GNUInstallDirs)
install(TARGETS treecheck
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
