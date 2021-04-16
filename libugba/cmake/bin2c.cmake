# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020-2021 Antonio Niño Díaz

function(add_data_file source_file destination_target)

    set(DATA_FILE ${source_file})

    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${source_directory}")

    # First, get full path of the file without extension

    get_filename_component(DATA_DIR ${DATA_FILE} DIRECTORY)
    get_filename_component(DATA_NAME_RAW ${DATA_FILE} NAME)

    # Replace dots by underscores

    string(REGEX REPLACE "\\." "_" DATA_NAME ${DATA_NAME_RAW})

    # Generate the output file names

    set(OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${source_directory}")
    set(OUT_C_FILE "${OUT_DIR}/${DATA_NAME}.c")
    set(OUT_H_FILE "${OUT_DIR}/${DATA_NAME}.h")

    set(OUT_FILES ${OUT_C_FILE} ${OUT_H_FILE})

    # Create rule that depends on the DATA and GRIT file

    add_custom_command(
        OUTPUT ${OUT_FILES}
        COMMAND $<TARGET_FILE:bin2c> ${DATA_FILE} ${OUT_DIR}
        DEPENDS ${DATA_FILE}
        WORKING_DIRECTORY ${OUT_DIR}
    )

    # Add output source files to the target

    message(VERBOSE "${destination_target}: Adding ${DATA_FILE}")

    target_sources(${destination_target} PRIVATE ${OUT_FILES})
    target_include_directories(${destination_target} PRIVATE ${OUT_DIR})
    set_source_files_properties(${OUT_FILES} PROPERTIES GENERATED 1)

endfunction()

function(add_data_files source_directory destination_target)

    file(GLOB DATA_FILES "${source_directory}/*")

    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${source_directory}")

    foreach(DATA_FILE ${DATA_FILES})
        add_data_file(${DATA_FILE} ${destination_target})
    endforeach()
endfunction()
