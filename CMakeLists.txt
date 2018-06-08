cmake_minimum_required (VERSION 3.5)
project (STACKTR)

# place result by GNU standards
set (CMAKE_LIBRARY_OUTPUT_DIRECTORy ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# project version
set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)
set (VERSION_PATCH 1)

# change system prefix
set (CMAKE_SYSTEM_PREFIX "/home/ytliu/.conda/envs/loc")

# set compiler flags
add_compile_options(-std=c++11)
set (CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -DDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE "-O2")

add_subdirectory(src)
