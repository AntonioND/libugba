# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020-2021 Antonio Niño Díaz

add_subdirectory(tools)

if(NOT LIBUGBA_ONLY)

    # GiiBiiAdvance is needed for unit tests

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

    # UMOD Player

    if("${UMOD_PLAYER_PATH}" STREQUAL "")
        get_filename_component(UMOD_PLAYER_PATH
            ../../umod-player ABSOLUTE
        )
    endif()

    if(EXISTS ${UMOD_PLAYER_PATH})
        add_subdirectory(${UMOD_PLAYER_PATH} umod_player)
    else()
        message(FATAL_ERROR "UMOD Player repository not found")
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
