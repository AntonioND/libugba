# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

cmake_minimum_required(VERSION 3.15)
project(gbaline)
enable_language(C ASM)

set(C_STANDARD 11)
set(C_EXTENSIONS OFF)
set(C_STANDARD_REQUIRED ON)

include(cmake/grit.cmake)
include(cmake/example.cmake)

add_subdirectory(libgbaline)

add_subdirectory(examples)
add_subdirectory(template)
