# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020-2021 Antonio Niño Díaz

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

# Save it to a regular variable so that it can be disabled per-folder.
set(BUILD_GBA_INTERNAL "${BUILD_GBA}")

# Global compiler-specific options
# --------------------------------

include(cmake/compiler_flags.cmake)

# Add subdirectories
# ------------------

add_subdirectory(libugba)

# Add testing
# -----------

# If this project is being used as a module within another project, remove all
# testing and examples from the build.
if(NOT PROJECT_IS_SUBMODULE)
    enable_testing()

    add_subdirectory(tools)

    if(NOT LIBUGBA_ONLY)

        # GiiBiiAdvance is needed for unit tests

        if(BUILD_GBA_INTERNAL)
            # Get absolute path to default folder
            if("${GIIBIIADVANCE_PATH}" STREQUAL "")
                get_filename_component(GIIBIIADVANCE_PATH
                    ../giibiiadvance ABSOLUTE
                )
            endif()

            if(EXISTS ${GIIBIIADVANCE_PATH})
                set(ENABLE_CAMERA OFF CACHE BOOL "")
                add_subdirectory(${GIIBIIADVANCE_PATH} giibiiadvance)
            else()
                message(FATAL_ERROR "GiiBiiAdvance repository not found")
            endif()
        endif()

        # UMOD Player

        if("${UMOD_PLAYER_PATH}" STREQUAL "")
            get_filename_component(UMOD_PLAYER_PATH
                ../umod-player ABSOLUTE
            )
        endif()

        if(EXISTS ${UMOD_PLAYER_PATH})
            add_subdirectory(${UMOD_PLAYER_PATH} umod_player)
        else()
            message(FATAL_ERROR "UMOD Player repository not found")
        endif()

    endif()

    include(cmake/example.cmake)

    add_subdirectory(examples)
    add_subdirectory(unittests)
endif()
