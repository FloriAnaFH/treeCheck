cmake_minimum_required(VERSION 3.10)
project(treecheck)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# ── Core library (no main) ────────────────────────────────────────────────────
add_library(treecheck_lib
    src/tree.cpp
    src/stats.cpp
    src/modes.cpp
)

# ── Main executable ───────────────────────────────────────────────────────────
add_executable(treecheck
    main.cpp
)
target_link_libraries(treecheck PRIVATE treecheck_lib)

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
