# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

# This makes GCC not export the symbols in a shared library by default. This is
# the behaviour in MSVC, so this makes it behave the same way in both compilers.
set(CMAKE_C_VISIBILITY_PRESET hidden)

add_library(libugba SHARED)

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

target_sources(libugba PRIVATE
    ${FILES_SOURCE}
    ${FILES_SOURCE_SDL2}
    ${FILES_SOURCE_SDL2_CORE}
    ${FILES_SOURCE_SDL2_GUI}
)

target_include_directories(libugba PUBLIC
    include
)

# Link with libraries and check build options
# -------------------------------------------

# libpng, SLD2 and Lua are required

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(libugba PRIVATE
        png
        SDL2::SDL2 SDL2::SDL2main
    )
else()
    target_include_directories(libugba PRIVATE
        ${PNG_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
    )
    target_link_libraries(libugba PRIVATE
        ${PNG_LIBRARIES}
        ${SDL2_LIBRARIES}
    )
endif()

target_link_libraries(libugba PRIVATE ${LUA_LIBRARIES})
target_include_directories(libugba PRIVATE ${LUA_INCLUDE_DIR})

# OpenGL is optional. It can be used as library to output graphics.

if(ENABLE_OPENGL)
    target_compile_definitions(libugba PRIVATE -DENABLE_OPENGL)
    target_include_directories(libugba PRIVATE ${OPENGL_INCLUDE_DIRS})
    target_link_libraries(libugba PRIVATE ${OPENGL_LIBRARIES})
endif()

# Add tools needed by libugba

add_subdirectory(tools)

# Add utilities to work with other tools like GRIT

include(cmake/bin2c.cmake)
include(cmake/grit.cmake)

# Add graphics to library, like the default font

add_grit_files(graphics libugba)
