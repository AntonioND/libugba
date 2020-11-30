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

# Build options
# -------------

option(BUILD_GBA "Build GBA binaries for testing in addition to PC" OFF)

# Global compiler-specific options
# --------------------------------

include(cmake/compiler_flags.cmake)

# Add subdirectories
# ------------------

add_subdirectory(libugba)

if(NOT LIBUGBA_ONLY)
    enable_testing()

    add_subdirectory(examples)
endif()
