# TreeCheck

A command-line tool that builds a Binary Search Tree (BST) from a text file and analyses it for AVL properties. It supports optional AVL self-balancing via rotations, key/subtree search, and a terminal animation mode that visualises each insertion step.

---

## Features

| Mode | Description |
|------|-------------|
| **Analysis** | Build a BST, report balance factors, AVL validity, and key statistics |
| **Rebalance** | Same as above but self-balances via AVL rotations after every insertion |
| **Search** | Look up a single key or check whether a second tree is a subtree of the first |
| **Animate** | Step-by-step terminal animation of insertions with colour-coded balance violations |

---

## Project Structure

```
treecheck/
├── main.cpp               # Entry point – CLI argument parsing, mode dispatch
├── CMakeLists.txt         # CMake build definition
├── Makefile               # Convenience wrapper around CMake
├── include/
│   ├── node.h             # BST node definition
│   ├── tree.h             # Tree class (insert, balance, search, file I/O)
│   ├── stats.h            # Aggregate statistics (min, max, avg)
│   ├── modes.h            # analysisMode / searchMode declarations
│   └── visualiser.h       # drawTree / animateMode declarations
├── src/
│   ├── tree.cpp           # Tree implementation (rotations, AVL checks, parsing)
│   ├── stats.cpp          # Stats implementation
│   ├── modes.cpp          # Analysis and search mode logic
│   └── visualiser.cpp     # Terminal drawing and animation
├── tests/
│   ├── test_all.cpp       # Unit tests (insert, balance, search, stats, file I/O)
│   ├── input1.txt         # Sample input
│   ├── avl_tree.txt       # Valid AVL tree input
│   ├── non_avl_tree.txt   # Invalid AVL tree input
│   ├── duplicates.txt     # Input containing duplicate keys
│   ├── single_key.txt     # Single-key input
│   └── invalid.txt        # Malformed input for error-handling tests
└── docs/
    └── protokoll.md       # Project protocol / notes
```

---

## Requirements

- CMake >= 3.10
- A C++17-capable compiler (g++ or clang++)

---

## Build

### Using CMake directly

```bash
mkdir build
cd build
cmake ..
make
```

The executable will be at `build/treecheck`.

### Using the Makefile wrapper

```bash
make        # configure + build
make clean  # remove build artefacts
```

### VSCode CMake integration

1. Open Settings (`Ctrl/Cmd + ,`) and search for `cmake.cmakePath`.
2. Set the path to your CMake binary (find it with `which cmake`, e.g. `/opt/homebrew/bin/cmake`).
3. Use the CMake Tools extension to configure and build.

---

## Input File Format

Each line of the input file must contain exactly one integer key:

```
10
5
15
3
7
```

- Blank lines and leading/trailing whitespace are ignored.
- Duplicate keys are silently skipped (the tree remains unchanged).
- Any line that cannot be parsed as an integer causes the program to exit with an error.

---

## Usage

```
treecheck [--rebalance] [--animate] <file> [<query-file>]
```

### Modes

#### Analysis – single file

Builds a BST from `<file>`, prints the balance factor of every node, reports whether the tree is AVL-valid, and prints key statistics (min, max, average).

```bash
./build/treecheck tests/input1.txt
```

#### Analysis with AVL rebalancing

Same output as above, but the tree self-balances after every insertion so the result is always AVL-valid.

```bash
./build/treecheck --rebalance tests/input1.txt
```

#### Search / subtree check – two files

When a second file is supplied, the query tree is compared against the main tree:

- **Single key** in the query file → path search; prints the traversal path if found.
- **Multiple keys** in the query file → subtree check; reports whether the query tree appears as a subtree in the main tree.

```bash
# Key search
./build/treecheck tests/input1.txt tests/single_key.txt

# Subtree check
./build/treecheck tests/input1.txt tests/avl_tree.txt

# With rebalancing
./build/treecheck --rebalance tests/input1.txt tests/avl_tree.txt
```

#### Animate

Animates each insertion step in the terminal. Nodes with a balance factor outside `[-1, 1]` are highlighted in red; the most recently inserted node is highlighted in yellow. After all insertions the final analysis and statistics are printed.

```bash
# Plain BST (violations shown in red)
./build/treecheck --animate tests/input1.txt

# AVL mode (rotations applied, always balanced)
./build/treecheck --rebalance --animate tests/input1.txt
```

> **Note:** `--animate` requires exactly one file argument.

---

## Running Tests

Tests are built alongside the main executable. To run them:

```bash
cd build
ctest --output-on-failure
```

Or run the test binary directly for more verbose output:

```bash
./build/treecheck_tests
```

Test coverage includes:

- Insertion and duplicate handling
- Balance-factor computation and AVL detection
- Single-key lookup and path reporting
- Tree equality and subtree containment
- File parsing (valid, invalid, empty, single-key, duplicate inputs)
- Statistics (min, max, average)