# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

add_subdirectory(tools)

# GiiBiiAdvance is needed for unit tests

if(NOT LIBUGBA_ONLY)
    if(BUILD_GBA)
        # Get absolute path to default folder
        if("${GIIBIIADVANCE_PATH}" STREQUAL "")
            get_filename_component(GIIBIIADVANCE_PATH
                ../../giibiiadvance ABSOLUTE
            )
        endif()

        if(EXISTS ${GIIBIIADVANCE_PATH})
            set(ENABLE_CAMERA OFF CACHE BOOL "")
            add_subdirectory(${GIIBIIADVANCE_PATH} giibiiadvance)
        else()
            message(FATAL_ERROR "GiiBiiAdvance repository not found")
        endif()
    endif()
endif()

include(cmake/example.cmake)

add_subdirectory(bios)
add_subdirectory(dma)
add_subdirectory(graphics)
add_subdirectory(input)
add_subdirectory(interrupts)
add_subdirectory(sound)
add_subdirectory(timers)
add_subdirectory(other)
