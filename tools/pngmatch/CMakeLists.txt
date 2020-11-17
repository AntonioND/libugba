# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

cmake_minimum_required(VERSION 3.15)
project(pngmatch)
enable_language(C)

set(C_STANDARD 11)
set(C_EXTENSIONS OFF)
set(C_STANDARD_REQUIRED ON)

add_executable(pngmatch)

target_sources(pngmatch PRIVATE pngmatch.c)

# libpng is required

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    find_package(libpng REQUIRED 1.6)
    target_link_libraries(pngmatch PRIVATE png)
else()
    find_package(PNG REQUIRED 1.6)
    target_include_directories(pngmatch PRIVATE ${PNG_INCLUDE_DIRS})
    target_link_libraries(pngmatch PRIVATE ${PNG_LIBRARIES})
endif()
