# treeCheck

Reads a text file containing integers (one per line), builds a binary search tree from them, and analyses it.

---

## Prerequisites

| Tool | Minimum version | Notes |
|------|----------------|-------|
| CMake | 3.15 | |
| C++ compiler | C++17 support required | GCC ≥ 7, Clang ≥ 5, MSVC 2017 (19.14) or later |
| Build backend | — | `make` / `ninja` on Linux; MSBuild or `ninja` on Windows |
| **Cross-compile only** | | |
| MinGW-w64 | GCC ≥ 9 recommended | `sudo apt install mingw-w64` (Debian/Ubuntu) |

---

## Build

All commands use CMake's `-B <dir>` style so each configuration gets its own directory and nothing needs to be `cd`-ed into.

### Linux

```sh
cmake -B build
cmake --build build
```

To run the tests:

```sh
ctest --test-dir build --output-on-failure
```

### Windows (Visual Studio)

Open a **Developer Command Prompt** (or any terminal with the MSVC environment loaded), then:

```bat
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

### Windows (Ninja / MinGW)

```bat
cmake -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

### Cross-compile for Windows on Linux (MinGW-w64)

This produces a native Windows `.exe` from a Linux host — no Windows machine needed.

Install the toolchain first:

```sh
sudo apt install mingw-w64          # Debian / Ubuntu
sudo dnf install mingw64-gcc-c++    # Fedora / RHEL
sudo pacman -S mingw-w64-gcc        # Arch
```

Then build:

```sh
cmake -B build-win -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw-w64.cmake
cmake --build build-win
```

The binary lands at `build-win/treecheck.exe`. The MinGW runtime is linked statically, so the file runs on any 64-bit Windows machine without extra DLLs.

> **Tip:** Use a separate build directory (e.g. `build-win`) so the cross-compiled and native builds don't interfere with each other.

### Debug builds

Pass `-DCMAKE_BUILD_TYPE=Debug` to any single-config generator (Make, Ninja) to enable debug symbols and disable optimisations:

```sh
cmake -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug
```

For multi-config generators (Visual Studio, Xcode), swap `--config Release` for `--config Debug` at build time instead.

### Install

```sh
cmake --install build         --prefix /usr/local      # Linux
cmake --install build         --prefix C:\treecheck    # Windows (native)
cmake --install build-win     --prefix ./dist-win      # Windows (cross-compiled)
```

The `treecheck` binary is placed in `<prefix>/bin/`.

---

## Usage

### Analysis mode

Reads a tree file, prints the balance factor of every node, reports whether the tree is AVL-valid, and prints basic statistics (min, max, average).

```sh
./treecheck <tree.txt>
```

Example output:

```
bal(5) = 1
bal(3) = 0
bal(7) = -1 (AVL violation)
AVL: NO
min: 3 | max: 7 | avg: 5
```

### Search mode

Provide a main tree file and a query file.

- **Single key in query file** – searches for the key in the main tree and prints the traversal path.
- **Multiple keys in query file** – checks whether the query tree is a (sparse) subtree of the main tree.

```sh
./treecheck <main_tree.txt> <query.txt>
```

Example output (single key):

```
5 found 8, 3, 5
```

Example output (subtree):

```
Subtree found
```

---

## Project structure

```
treeCheck/
├── CMakeLists.txt        # Cross-platform build definition
├── main.cpp              # Entry point
├── cmake/
│   └── toolchain-mingw-w64.cmake  # Linux → Windows cross-compilation toolchain
├── include/
│   ├── modes.h           # analysisMode / searchMode declarations
│   ├── tree.h            # Binary search tree
│   ├── node.h            # Tree node
│   └── stats.h           # Statistics helper
├── src/
│   ├── modes.cpp         # Mode implementations
│   ├── tree.cpp          # Tree implementation
│   └── stats.cpp         # Stats implementation
├── tests/
│   ├── test_all.cpp      # Test suite
│   ├── avl_tree.txt      # Sample AVL tree input
│   ├── non_avl_tree.txt  # Sample non-AVL tree input
│   ├── single_key.txt    # Single-key query sample
│   ├── duplicates.txt    # Duplicate-key sample
│   ├── invalid.txt       # Invalid input sample
│   └── input1.txt        # General test input
└── docs/
```

---

## Input file format

Plain text, one integer per line. Blank lines and leading/trailing whitespace are ignored. Duplicate keys are silently skipped.

```
8
3
10
1
5
```
