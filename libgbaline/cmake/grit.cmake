# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

# Default Grit path
set(GRIT_PATH "$ENV{DEVKITPRO}/tools/bin/grit"
    CACHE STRING "Path to the Grit executable")

function(add_grit_files source_directory destination_target)

    file(GLOB PNG_FILES "${source_directory}/*.png")
    file(GLOB GIF_FILES "${source_directory}/*.gif")
    file(GLOB JPG_FILES "${source_directory}/*.jpg")

    set(GFX_FILES ${PNG_FILES} ${GIF_FILES} ${JPG_FILES})

    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${source_directory}")

    foreach(GFX_FILE ${GFX_FILES})

        # First, get full path of the file without extension

        get_filename_component(GFX_DIR ${GFX_FILE} DIRECTORY)
        get_filename_component(GFX_NAME ${GFX_FILE} NAME_WLE)

        set(PATH_NO_EXT "${GFX_DIR}/${GFX_NAME}")

        # Now, check that there is a .grit file along the graphics file

        set(GRIT_FILE "${PATH_NO_EXT}.grit")

        if(NOT EXISTS ${GRIT_FILE})
            message(FATAL_ERROR "GFX file without GRIT file: ${GFX_FILE}")
        endif()

        # Generate the output file names

        set(OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${source_directory}")
        set(OUT_C_FILE "${OUT_DIR}/${GFX_NAME}.c")
        set(OUT_H_FILE "${OUT_DIR}/${GFX_NAME}.h")

        set(OUT_FILES ${OUT_C_FILE} ${OUT_H_FILE})

        # Create rule that depends on the GFX and GRIT file

        add_custom_command(
            OUTPUT ${OUT_FILES}
            COMMAND ${GRIT_PATH} ${GFX_FILE} -ftc -o ${GFX_NAME}
            DEPENDS ${GRIT_FILE} ${GFX_FILE}
            WORKING_DIRECTORY ${OUT_DIR}
        )

        # Add output source files to the target

        message(VERBOSE "${destination_target}: Adding ${GFX_FILE}")

        target_sources(${destination_target} PRIVATE ${OUT_FILES})
        target_include_directories(${destination_target} PRIVATE ${OUT_DIR})
        set_source_files_properties(${OUT_FILES} PROPERTIES GENERATED 1)

    endforeach()
endfunction()
