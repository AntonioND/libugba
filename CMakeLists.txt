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

get_directory_property(PROJECT_HAS_PARENT PARENT_DIRECTORY)
set(PROJECT_IS_SUBMODULE ${PROJECT_HAS_PARENT})

# Build options
# -------------

option(BUILD_GBA "Build GBA binaries in addition to PC binaries" OFF)

# Global compiler-specific options
# --------------------------------

include(cmake/compiler_flags.cmake)

# Add subdirectories
# ------------------

add_subdirectory(libugba)

# If this project is being used as a module within another project, remove all
# testing and examples from the build.
if(NOT PROJECT_IS_SUBMODULE)
    enable_testing()

    add_subdirectory(examples)
endif()
