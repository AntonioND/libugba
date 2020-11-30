# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

cmake_minimum_required(VERSION 3.15)
project(ugba)
enable_language(C ASM)

set(C_STANDARD 11)
set(C_EXTENSIONS OFF)
set(C_STANDARD_REQUIRED ON)

# Detect if this project is a submodule
# -------------------------------------

# If this project is being used as a module within another project, remove all
# testing and examples from the build.
get_directory_property(HAS_PARENT PARENT_DIRECTORY)
if(HAS_PARENT)
    set(LIBUGBA_ONLY ON)
else()
    set(LIBUGBA_ONLY OFF)
endif()

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

# Global compiler-specific options
# --------------------------------

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(
        # Force all integers to be 2's complement to prevent the compiler from
        # doing optimizations because of undefined behaviour.
        -fwrapv

        # Force usage of extern for external variables
        -fno-common

        # Enable most common warnings
        -Wall -Wextra

        # Disable this warning, which is enabled by default
        -Wformat-truncation=0
    )
    if(CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 9.3)
        add_compile_options(
            # Enable a bunch of warnings that aren't enabled with Wall or Wextra
            -Wformat-overflow=2 -Wformat=2 -Wno-format-nonliteral
            -Wundef -Wunused -Wuninitialized -Wunknown-pragmas -Wshadow
            -Wlogical-op -Wduplicated-cond -Wswitch-enum -Wfloat-equal
            -Wcast-align -Walloc-zero -Winline
            -Wstrict-overflow=5 -Wstringop-overflow=4
            $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>
            $<$<COMPILE_LANGUAGE:C>:-Wold-style-definition>

            # Enable Wpedantic but disable warning about having strings that are
            # too long
            -Wpedantic -Wno-overlength-strings

            # Make sure we don't use too much stack. Windows doesn't like it
            # when the stack usage is too high, even when Linux doesn't complain
            # about it.
            -Wstack-usage=4096

            # TODO: Enable the following warnings?
            #-Wformat-truncation=1 -Wcast-qual -Wconversion
        )

        # Build option to enable Undefined Behaviour Sanitizer (UBSan)
        # --------------------------------------------------------
        #
        # This should only be enabled in debug builds. It makes the code far
        # slower, so it should only be used during development.
        option(ENABLE_UBSAN "Compile with UBSan support (GCC)" OFF)

        if(ENABLE_UBSAN)
            add_compile_options(-fsanitize=undefined)
            add_link_options(-fsanitize=undefined)
        endif()
    endif()
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    add_compile_definitions(
        # Silence warnings
        _USE_MATH_DEFINES
        _CRT_SECURE_NO_WARNINGS
    )
    add_compile_options(
        # Enable parallel compilation
        /MP
    )
endif()

# Add subdirectories
# ------------------

add_subdirectory(libugba)

if(NOT LIBUGBA_ONLY)
    enable_testing()

    add_subdirectory(examples)
    add_subdirectory(template)
endif()
