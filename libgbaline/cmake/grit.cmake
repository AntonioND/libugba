# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

function(add_grit_files source_directory destination_target)
    set(GRIT "$ENV{DEVKITPRO}/tools/bin/grit")

    file(GLOB PNG_FILES "${source_directory}/*.png")

    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${source_directory}")

    foreach(PNG_FILE ${PNG_FILES})

        # First, get full path of the file without extension

        get_filename_component(PNG_DIR ${PNG_FILE} DIRECTORY)
        get_filename_component(PNG_NAME ${PNG_FILE} NAME_WLE)

        set(PATH_NO_EXT "${PNG_DIR}/${PNG_NAME}")

        # Now, check that there is a .grit file along the .png file

        set(GRIT_FILE "${PATH_NO_EXT}.grit")

        if(NOT EXISTS ${GRIT_FILE})
            message(FATAL_ERROR "PNG file without GRIT file: ${PNG_FILE}")
        endif()

        # Generate the output file names

        set(OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${source_directory}")
        set(OUT_C_FILE "${OUT_DIR}/${PNG_NAME}.c")
        set(OUT_H_FILE "${OUT_DIR}/${PNG_NAME}.h")

        set(OUT_FILES ${OUT_C_FILE} ${OUT_H_FILE})

        # Create rule that depends on the PNG and GRIT file

        add_custom_command(
            OUTPUT ${OUT_FILES}
            COMMAND ${GRIT} ${PNG_FILE} -ftc -o ${PNG_NAME}
            DEPENDS ${GRIT_FILE} ${PNG_FILE}
            WORKING_DIRECTORY ${OUT_DIR}
        )

        # Add output source files to the target

        message(VERBOSE "${destination_target}: Adding ${PNG_NAME}.png")

        target_sources(${destination_target} PRIVATE ${OUT_FILES})
        target_include_directories(${destination_target} PRIVATE ${OUT_DIR})
        set_source_files_properties(${OUT_FILES} PROPERTIES GENERATED 1)

    endforeach()
endfunction()
