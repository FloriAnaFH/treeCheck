cmake_minimum_required(VERSION 3.10)
project(treecheck)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(treecheck
    src/main.cpp
    src/tree.cpp
)