# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020-2022 Antonio Niño Díaz

cmake_minimum_required(VERSION 3.15)
project(libugba)
enable_language(C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Global library settings
# -----------------------

set(LIBRARY_NAME libugba)

# Global build options
# --------------------

option(USE_DEVKITARM "Use devkitARM to build GBA binaries" ON)

option(ENABLE_DEBUG_CHECKS "Enable debug checks in the library." ON)

# Option to enable the debugger windows in the SDL port. It requires libpng to
# dump images.
option(ENABLE_DEBUGGER "Support debugger windows (I/O registers, VRAM)" ON)

# Toolchain utilities
# -------------------

get_filename_component(LIBSYSGBA_PATH_local "libsysgba" ABSOLUTE)
set(LIBSYSGBA_PATH "${LIBSYSGBA_PATH_local}" CACHE STRING "" FORCE)

include(cmake/compiler_flags.cmake)
include(cmake/toolchain_gba.cmake)
include(cmake/toolchain_sdl2.cmake)
include(cmake/toolchain_gba_helpers.cmake)

# Look for an ARM toolchain. If this library is included by a parent project,
# check if the path has already been specified.

if(NOT "${BUILD_GBA}" STREQUAL OFF)
    if("${ARM_GCC_PATH}" STREQUAL "")
        message(STATUS "libugba: GBA toolchain not specified")
        ugba_search_toolchain_gba(BUILD_GBA)
    else()
        message(STATUS "libugba: GBA toolchain has been specified")
        set(BUILD_GBA ON)
    endif()
endif()

if(BUILD_GBA)
    message(STATUS "libugba: GBA toolchain: ${ARM_GCC_PATH}")
endif()

# Add tools needed by libugba
# ---------------------------

add_subdirectory(tools)

# Add libsysgba module
# --------------------

if(NOT USE_DEVKITARM)
    if(BUILD_GBA)
        add_subdirectory(libsysgba)
    endif()
endif()

# Add source code files
# ---------------------

# Macro that searches all the source files in the specified directory in 'dir'
# and saves them in 'var'
macro(ugba_search_source_files dir var)
    file(GLOB ${var} CONFIGURE_DEPENDS ${dir}/*.c ${dir}/*.h ${dir}/*.s)
endmacro()

ugba_search_source_files(source FILES_SOURCE)
ugba_search_source_files(source/graphics FILES_SOURCE_GRAPHICS)
ugba_search_source_files(source/gba FILES_SOURCE_GBA)
ugba_search_source_files(source/sdl2 FILES_SOURCE_SDL2)
ugba_search_source_files(source/sdl2/core FILES_SOURCE_SDL2_CORE)
ugba_search_source_files(source/sdl2/gui FILES_SOURCE_SDL2_GUI)
if(ENABLE_DEBUGGER)
    ugba_search_source_files(source/sdl2/gui/debugger
                             FILES_SOURCE_SDL2_GUI_DEBUGGER)
else()
    set(FILES_SOURCE_SDL2_GUI_DEBUGGER "")
endif()

set(SOURCES_GBA
    ${FILES_SOURCE}
    ${FILES_SOURCE_GBA}
    ${FILES_SOURCE_GRAPHICS}
)

set(SOURCES_SDL2
    ${FILES_SOURCE}
    ${FILES_SOURCE_GRAPHICS}
    ${FILES_SOURCE_SDL2}
    ${FILES_SOURCE_SDL2_CORE}
    ${FILES_SOURCE_SDL2_GUI}
    ${FILES_SOURCE_SDL2_GUI_DEBUGGER}
)

get_filename_component(INCLUDE_PATH "include" ABSOLUTE)

add_subdirectory(source)
