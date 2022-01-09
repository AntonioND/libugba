# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020-2022 Antonio Niño Díaz

cmake_minimum_required(VERSION 3.15)
project(libsysgba)
enable_language(C ASM)

set(C_STANDARD 11)
set(C_EXTENSIONS OFF)
set(C_STANDARD_REQUIRED ON)

# Global library settings
# -----------------------

set(LIBRARY_NAME libsysgba)

# Toolchain selection macros
# --------------------------

get_filename_component(LIBSYSGBA_PATH_local "." ABSOLUTE)

set(LIBSYSGBA_PATH "${LIBSYSGBA_PATH_local}" CACHE STRING "" FORCE)

macro(toolchain_gba)
    include(${LIBSYSGBA_PATH}/cmake/toolchain_gba.cmake)
endmacro()

# Add utilities to work with other tools
# --------------------------------------

include(cmake/toolchain_gba_helpers.cmake)

# Add source code files
# ---------------------

add_subdirectory(source)
