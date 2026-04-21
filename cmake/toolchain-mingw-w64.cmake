# cmake/toolchain-mingw-w64.cmake
#
# Cross-compilation toolchain for targeting 64-bit Windows from Linux
# using the MinGW-w64 toolchain.
#
# Install the toolchain:
#   Debian/Ubuntu:  sudo apt install mingw-w64
#   Fedora/RHEL:    sudo dnf install mingw64-gcc-c++
#   Arch:           sudo pacman -S mingw-w64-gcc
#
# Usage (from the project root):
#   cmake -B build-win -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw-w64.cmake
#   cmake --build build-win

# ── Target system ─────────────────────────────────────────────────────────────
set(CMAKE_SYSTEM_NAME     Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# ── Toolchain prefix ──────────────────────────────────────────────────────────
# Adjust this if your distro uses a different triple (e.g. x86_64-w64-mingw32).
set(MINGW_PREFIX x86_64-w64-mingw32)

set(CMAKE_C_COMPILER   ${MINGW_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${MINGW_PREFIX}-g++)
set(CMAKE_RC_COMPILER  ${MINGW_PREFIX}-windres)

# ── Sysroot ───────────────────────────────────────────────────────────────────
# Point CMake at the MinGW sysroot so find_package / find_library search there.
set(CMAKE_FIND_ROOT_PATH /usr/${MINGW_PREFIX})

# Programs (e.g. cmake scripts) come from the host; headers and libs from the target.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
