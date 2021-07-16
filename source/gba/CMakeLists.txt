# SPDX-License-Identifier: MIT
#
# Copyright (c) 2021 Antonio Niño Díaz

include(${CMAKE_SOURCE_DIR}/cmake/toolchain_gba.cmake)

# Define library target
# ---------------------

add_library(${LIBRARY_NAME}_gba STATIC)

# Source code, include directories and global definitions
# -------------------------------------------------------

target_sources(${LIBRARY_NAME}_gba PRIVATE ${SOURCES_GBA})
target_include_directories(${LIBRARY_NAME}_gba PUBLIC ${INCLUDE_PATH})

if(ENABLE_DEBUG_CHECKS)
    target_compile_definitions(${LIBRARY_NAME}_gba PRIVATE -DUGBA_DEBUG)
endif()

