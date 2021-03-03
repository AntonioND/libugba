# SPDX-License-Identifier: MIT
#
# Copyright (c) 2021 Antonio Niño Díaz

function(add_umod_player_files src_dir destination_target dest_bin dest_header)

    file(GLOB MOD_FILES "${src_dir}/*.mod")
    file(GLOB WAV_FILES "${src_dir}/*.wav")

    set(AUDIO_FILES ${MOD_FILES} ${WAV_FILES})

    message(VERBOSE "${destination_target}: Adding ${AUDIO_FILES}")

    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${src_dir}")

    # Generate the output file names

    set(OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/${src_dir}")
    set(OUT_BIN "${OUT_DIR}/${dest_bin}")
    set(OUT_HEADER "${OUT_DIR}/${dest_header}")

    set(OUT_FILES ${OUT_BIN} ${OUT_HEADER})

    # Create rule that depends on the audio files

    add_custom_command(
        OUTPUT ${OUT_FILES}
        COMMAND $<TARGET_FILE:umod_packer> ${OUT_BIN} ${OUT_HEADER} ${AUDIO_FILES}
        COMMAND $<TARGET_FILE:bin2c> ${OUT_BIN} ${OUT_DIR}
        DEPENDS ${AUDIO_FILES}
        WORKING_DIRECTORY ${OUT_DIR}
    )

    # Add output header file to the target

    target_sources(${destination_target} PRIVATE ${OUT_HEADER})
    target_include_directories(${destination_target} PRIVATE ${OUT_DIR})
    set_source_files_properties(${OUT_HEADER} PROPERTIES GENERATED 1)

    # Add output bin file to the target

    add_data_file(${OUT_BIN} ${destination_target})

endfunction()
