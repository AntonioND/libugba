# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020-2021 Antonio Niño Díaz

cmake_minimum_required(VERSION 3.15)
project(libugba)
enable_language(C ASM)

set(C_STANDARD 11)
set(C_EXTENSIONS OFF)
set(C_STANDARD_REQUIRED ON)

# Global compiler-specific options
# --------------------------------

include(cmake/compiler_flags.cmake)

# Global library settings
# -----------------------

set(LIBRARY_NAME libugba)

# This makes GCC not export the symbols in a shared library by default. This is
# the behaviour in MSVC, so this makes it behave the same way in both compilers.
set(CMAKE_C_VISIBILITY_PRESET hidden)

add_library(${LIBRARY_NAME} SHARED)

# Link with libraries and check build options
# -------------------------------------------

# Option to enable debug checks in the library.

option(ENABLE_DEBUG_CHECKS "Enable debug checks in the library." ON)
if(ENABLE_DEBUG_CHECKS)
    target_compile_definitions(${LIBRARY_NAME} PRIVATE -DUGBA_DEBUG)
endif()

# Option to enable the debugger windows. It requires libpng to dump images.

option(ENABLE_DEBUGGER "Support debugger windows (I/O registers, VRAM)" ON)
if(ENABLE_DEBUGGER)
    target_compile_definitions(${LIBRARY_NAME} PRIVATE -DENABLE_DEBUGGER)
endif()

# Option to enable screenshots. It requires libpng.

option(ENABLE_SCREENSHOTS "Support taking screenshots" ON)
if(ENABLE_SCREENSHOTS)
    target_compile_definitions(${LIBRARY_NAME} PRIVATE -DENABLE_SCREENSHOTS)
endif()

# Lua is optional. It can be used to run automated scripts.

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    include(FindLua)
    find_package(Lua 5.2)
else()
    find_package(Lua 5.2)
endif()

# Option to enable the Lua interpreter. Requires screenshot support.

if (LUA_FOUND)
    option(LUA_INTERPRETER_ENABLED "Enable Lua interpreter" OFF)
else()
    set(LUA_INTERPRETER_ENABLED OFF)
endif()

if(LUA_INTERPRETER_ENABLED)
    target_compile_definitions(${LIBRARY_NAME} PRIVATE -DLUA_INTERPRETER_ENABLED)
    target_link_libraries(${LIBRARY_NAME} PRIVATE ${LUA_LIBRARIES})
    target_include_directories(${LIBRARY_NAME} PRIVATE ${LUA_INCLUDE_DIR})

    if(NOT ENABLE_SCREENSHOTS)
        message(FATAL_ERROR "ENABLE_SCREENSHOTS needed for LUA_INTERPRETER_ENABLED")
    endif()
endif()

# libpng is optional. It is needed to take screenshots and dump VRAM data.

if(ENABLE_SCREENSHOTS OR ENABLE_DEBUGGER OR LUA_INTERPRETER_ENABLED)
    if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        find_package(libpng REQUIRED 1.6)
        target_link_libraries(${LIBRARY_NAME} PRIVATE png)
    else()
        find_package(PNG REQUIRED 1.6)
        target_include_directories(${LIBRARY_NAME} PRIVATE ${PNG_INCLUDE_DIRS})
        target_link_libraries(${LIBRARY_NAME} PRIVATE ${PNG_LIBRARIES})
    endif()
    target_compile_definitions(${LIBRARY_NAME} PRIVATE -DENABLE_LIBPNG)
endif()

# SLD2 is always required

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    find_package(SDL2 REQUIRED 2.0.7)
    target_link_libraries(${LIBRARY_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)
else()
    find_package(SDL2 REQUIRED 2.0.7)
    target_include_directories(${LIBRARY_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${LIBRARY_NAME} PRIVATE ${SDL2_LIBRARIES})
endif()

# OpenGL is optional. It can be used as library to output graphics.

find_package(OpenGL)

if(OPENGL_FOUND)
    option(ENABLE_OPENGL "Compile with OpenGL" ON)
else()
    set(ENABLE_OPENGL OFF)
endif()

if(ENABLE_OPENGL)
    target_compile_definitions(${LIBRARY_NAME} PRIVATE -DENABLE_OPENGL)
    target_include_directories(${LIBRARY_NAME} PRIVATE ${OPENGL_INCLUDE_DIRS})
    target_link_libraries(${LIBRARY_NAME} PRIVATE ${OPENGL_LIBRARIES})
endif()

# Add tools needed by libugba
# ---------------------------

add_subdirectory(tools)

# Add utilities to work with other tools like GRIT

include(cmake/bin2c.cmake)
include(cmake/grit.cmake)
include(cmake/umod-player.cmake)

# Add source code files
# ---------------------

# Macro that searches all the source files in the specified directory in 'dir'
# and saves them in 'var'
macro(search_source_files dir var)
    file(GLOB ${var} CONFIGURE_DEPENDS ${dir}/*.c ${dir}/*.h)
endmacro()

search_source_files(graphics FILES_GRAPHICS)
search_source_files(source FILES_SOURCE)
search_source_files(source/sdl2 FILES_SOURCE_SDL2)
search_source_files(source/sdl2/core FILES_SOURCE_SDL2_CORE)
search_source_files(source/sdl2/gui FILES_SOURCE_SDL2_GUI)
if(ENABLE_DEBUGGER)
    search_source_files(source/sdl2/gui/debugger FILES_SOURCE_SDL2_GUI_DEBUGGER)
else()
    set(FILES_SOURCE_SDL2_GUI_DEBUGGER "")
endif()

target_sources(${LIBRARY_NAME} PRIVATE
    ${FILES_GRAPHICS}
    ${FILES_SOURCE}
    ${FILES_SOURCE_SDL2}
    ${FILES_SOURCE_SDL2_CORE}
    ${FILES_SOURCE_SDL2_GUI}
    ${FILES_SOURCE_SDL2_GUI_DEBUGGER}
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    include
    graphics
)

# Build GBA version if requested
# ------------------------------

option(BUILD_GBA "Build GBA binaries in addition to PC binaries" OFF)

if(BUILD_GBA)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/lib/libugba.a
        BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/build
        COMMAND make -j`nproc`
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    add_custom_target(libugba_gba ALL
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lib/libugba.a
    )
endif()
