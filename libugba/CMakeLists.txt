# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

set(LIBRARY_NAME libugba)

# This makes GCC not export the symbols in a shared library by default. This is
# the behaviour in MSVC, so this makes it behave the same way in both compilers.
set(CMAKE_C_VISIBILITY_PRESET hidden)

add_library(${LIBRARY_NAME} SHARED)

# Add source code files
# ---------------------

# Macro that searches all the source files in the specified directory in 'dir'
# and saves them in 'var'
macro(search_source_files dir var)
    file(GLOB ${var} CONFIGURE_DEPENDS ${dir}/*.c ${dir}/*.h)
endmacro()

search_source_files(source FILES_SOURCE)
search_source_files(source/sdl2 FILES_SOURCE_SDL2)
search_source_files(source/sdl2/core FILES_SOURCE_SDL2_CORE)
search_source_files(source/sdl2/gui FILES_SOURCE_SDL2_GUI)

target_sources(${LIBRARY_NAME} PRIVATE
    ${FILES_SOURCE}
    ${FILES_SOURCE_SDL2}
    ${FILES_SOURCE_SDL2_CORE}
    ${FILES_SOURCE_SDL2_GUI}
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    include
)

# Link with libraries and check build options
# -------------------------------------------

# libpng, SLD2 and Lua are required

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    find_package(libpng REQUIRED 1.6)
    find_package(SDL2 REQUIRED)
    include(FindLua)
    find_package(Lua REQUIRED 5.2)

    target_link_libraries(${LIBRARY_NAME} PRIVATE
        png
        SDL2::SDL2 SDL2::SDL2main
    )
else()
    find_package(PNG REQUIRED 1.6)
    find_package(SDL2 REQUIRED)
    find_package(Lua REQUIRED 5.2)

    target_include_directories(${LIBRARY_NAME} PRIVATE
        ${PNG_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
    )
    target_link_libraries(${LIBRARY_NAME} PRIVATE
        ${PNG_LIBRARIES}
        ${SDL2_LIBRARIES}
    )
endif()

target_link_libraries(${LIBRARY_NAME} PRIVATE ${LUA_LIBRARIES})
target_include_directories(${LIBRARY_NAME} PRIVATE ${LUA_INCLUDE_DIR})

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

add_subdirectory(tools)

# Add utilities to work with other tools like GRIT

include(cmake/bin2c.cmake)
include(cmake/grit.cmake)

# Add graphics to library, like the default font

add_grit_files(graphics ${LIBRARY_NAME})

# Build GBA version if requested

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
