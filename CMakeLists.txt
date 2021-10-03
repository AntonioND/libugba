# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020-2021 Antonio Niño Díaz

cmake_minimum_required(VERSION 3.15)
project(libugba)
enable_language(C ASM)

set(C_STANDARD 11)
set(C_EXTENSIONS OFF)
set(C_STANDARD_REQUIRED ON)

# Global library settings
# -----------------------

set(LIBRARY_NAME libugba)

# Global build options
# --------------------

option(BUILD_GBA "Build GBA binaries in addition to PC binaries" OFF)

option(ENABLE_DEBUG_CHECKS "Enable debug checks in the library." ON)

# Option to enable the debugger windows in the SDL port. It requires libpng to
# dump images.
option(ENABLE_DEBUGGER "Support debugger windows (I/O registers, VRAM)" ON)

# Toolchain selection macros
# --------------------------

get_filename_component(LIBUGBA_PATH "." ABSOLUTE)

macro(toolchain_gba)
    include(${LIBUGBA_PATH}/cmake/toolchain_gba.cmake)
endmacro()

macro(toolchain_sdl2)
    include(${LIBUGBA_PATH}/cmake/toolchain_sdl2.cmake)
endmacro()

# Add tools needed by libugba
# ---------------------------

add_subdirectory(tools)

# Add utilities to work with other tools like GRIT

include(cmake/bin2c.cmake)
include(cmake/grit.cmake)
include(cmake/toolchain_gba_helpers.cmake)
include(cmake/umod-player.cmake)

# Add source code files
# ---------------------

# Macro that searches all the source files in the specified directory in 'dir'
# and saves them in 'var'
macro(search_source_files dir var)
    file(GLOB ${var} CONFIGURE_DEPENDS ${dir}/*.c ${dir}/*.h ${dir}/*.s)
endmacro()

search_source_files(source FILES_SOURCE)
search_source_files(source/graphics FILES_SOURCE_GRAPHICS)
search_source_files(source/gba FILES_SOURCE_GBA)
search_source_files(source/sdl2 FILES_SOURCE_SDL2)
search_source_files(source/sdl2/core FILES_SOURCE_SDL2_CORE)
search_source_files(source/sdl2/gui FILES_SOURCE_SDL2_GUI)
if(ENABLE_DEBUGGER)
    search_source_files(source/sdl2/gui/debugger FILES_SOURCE_SDL2_GUI_DEBUGGER)
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
