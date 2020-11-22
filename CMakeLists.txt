# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

cmake_minimum_required(VERSION 3.15)
project(ugba)
enable_language(C ASM)

set(C_STANDARD 11)
set(C_EXTENSIONS OFF)
set(C_STANDARD_REQUIRED ON)

# Find dependencies
# -----------------

# libpng, SLD2 and Lua are required

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    find_package(libpng REQUIRED 1.6)
    find_package(SDL2 REQUIRED)
    include(FindLua)
    find_package(lua REQUIRED 5.2)
else()
    find_package(PNG REQUIRED 1.6)
    find_package(SDL2 REQUIRED)
    find_package(Lua REQUIRED 5.2)
endif()

# OpenGL is optional. It can be used as library to output graphics.

find_package(OpenGL)

if(OPENGL_FOUND)
    option(ENABLE_OPENGL "Compile with OpenGL" ON)
else()
    set(ENABLE_OPENGL OFF)
endif()

# Add subdirectories
# ------------------

add_subdirectory(libugba)

# If this project is being used as a module within another project, don't add
# examples, template or anything that isn't needed.
get_directory_property(HAS_PARENT PARENT_DIRECTORY)
if(NOT HAS_PARENT)
    add_subdirectory(template)
    add_subdirectory(tools)

    enable_testing()

    add_subdirectory(examples)
endif()
