# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020-2021 Antonio Niño Díaz

function(define_example)

    # Get name of the folder we are in
    # --------------------------------

    get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    add_executable(${EXECUTABLE_NAME})

    target_link_libraries(${EXECUTABLE_NAME} libugba)

    # Add source code files
    # ---------------------

    # Macro that searches all the source files in the specified directory in
    # 'dir' and saves them in 'var'
    macro(search_source_files dir var)
        file(GLOB ${var} CONFIGURE_DEPENDS ${dir}/*.c ${dir}/*.h)
    endmacro()

    search_source_files(source FILES_SOURCE)

    target_sources(${EXECUTABLE_NAME} PRIVATE ${FILES_SOURCE})

    # Add graphics files
    # ------------------

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/graphics)
        add_grit_files(graphics ${EXECUTABLE_NAME})
    endif()

    # Add data files
    # --------------

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/data)
        add_data_files(data ${EXECUTABLE_NAME})
    endif()

    # Add audio files
    # ---------------

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/audio)
        target_link_libraries(${EXECUTABLE_NAME} umod_player)
        add_umod_player_files(audio ${EXECUTABLE_NAME}
            umod_pack.bin umod_pack_info.h
        )
    endif()

    # Build GBA version if requested
    # ------------------------------

    if(BUILD_GBA)
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${EXECUTABLE_NAME}.gba
            BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/build
            COMMAND make -j`nproc`
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )

        add_custom_target(${EXECUTABLE_NAME}_gba ALL
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${EXECUTABLE_NAME}.gba
        )

        add_dependencies(${EXECUTABLE_NAME}_gba libugba_gba)
    endif()

endfunction()

function(unittest_screenshot)

    # Get name of the folder we are in
    # --------------------------------

    get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    # SDL2 test
    # ---------

    set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test-sdl2.lua")
    if(NOT EXISTS ${TEST_SCRIPT})
        set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test.lua")
    endif()

    set(REF_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-sdl2.png")
    if(NOT EXISTS ${REF_PNG})
        set(REF_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference.png")
    endif()

    set(CMD1 "$<TARGET_FILE:${EXECUTABLE_NAME}> --lua ${TEST_SCRIPT}")
    set(CMD2 "$<TARGET_FILE:pngmatch> ${REF_PNG} screenshot.png")

    add_test(NAME ${EXECUTABLE_NAME}_test
        COMMAND ${CMAKE_COMMAND}
                    -DCMD1=${CMD1}
                    -DCMD2=${CMD2}
                    -P ${CMAKE_SOURCE_DIR}/examples/cmake/runcommands.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    # Emulator test
    # -------------

    if(BUILD_GBA)
        set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test-gba.lua")
        if(NOT EXISTS ${TEST_SCRIPT})
            set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test.lua")
        endif()

        set(REF_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-gba.png")
        if(NOT EXISTS ${REF_PNG})
            set(REF_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference.png")
        endif()

        set(GBA_ROM "${CMAKE_CURRENT_SOURCE_DIR}/${EXECUTABLE_NAME}.gba")

        set(CMD1 "$<TARGET_FILE:giibiiadvance> --lua ${TEST_SCRIPT} ${GBA_ROM}")
        set(CMD2 "$<TARGET_FILE:pngmatch> ${REF_PNG} screenshot.png")

        add_test(NAME ${EXECUTABLE_NAME}_gba_test
            COMMAND ${CMAKE_COMMAND}
                        -DCMD1=${CMD1}
                        -DCMD2=${CMD2}
                        -P ${CMAKE_SOURCE_DIR}/examples/cmake/runcommands.cmake
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()

endfunction()

function(unittest_two_screenshots)

    # Get name of the folder we are in
    get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test-sdl2.lua")
    if(NOT EXISTS ${TEST_SCRIPT})
        set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test.lua")
    endif()

    set(REF_1_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-1-sdl2.png")
    if(NOT EXISTS ${REF_1_PNG})
        set(REF_1_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-1.png")
    endif()

    set(REF_2_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-2-sdl2.png")
    if(NOT EXISTS ${REF_2_PNG})
        set(REF_2_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-2.png")
    endif()

    set(CMD1 "$<TARGET_FILE:${EXECUTABLE_NAME}> --lua ${TEST_SCRIPT}")
    set(CMD2 "$<TARGET_FILE:pngmatch> ${REF_1_PNG} screenshot-1.png")
    set(CMD3 "$<TARGET_FILE:pngmatch> ${REF_2_PNG} screenshot-2.png")

    add_test(NAME ${EXECUTABLE_NAME}_test
        COMMAND ${CMAKE_COMMAND}
                    -DCMD1=${CMD1}
                    -DCMD2=${CMD2}
                    -DCMD3=${CMD3}
                    -P ${CMAKE_SOURCE_DIR}/examples/cmake/runcommands.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    # Emulator test
    # -------------

    if(BUILD_GBA)
        set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test-gba.lua")
        if(NOT EXISTS ${TEST_SCRIPT})
            set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test.lua")
        endif()

        set(REF_1_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-1-gba.png")
        if(NOT EXISTS ${REF_1_PNG})
            set(REF_1_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-1.png")
        endif()

        set(REF_2_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-2-gba.png")
        if(NOT EXISTS ${REF_2_PNG})
            set(REF_2_PNG "${CMAKE_CURRENT_SOURCE_DIR}/reference-2.png")
        endif()

        set(GBA_ROM "${CMAKE_CURRENT_SOURCE_DIR}/${EXECUTABLE_NAME}.gba")

        set(CMD1 "$<TARGET_FILE:giibiiadvance> --lua ${TEST_SCRIPT} ${GBA_ROM}")
        set(CMD2 "$<TARGET_FILE:pngmatch> ${REF_1_PNG} screenshot-1.png")
        set(CMD3 "$<TARGET_FILE:pngmatch> ${REF_2_PNG} screenshot-2.png")

        add_test(NAME ${EXECUTABLE_NAME}_gba_test
            COMMAND ${CMAKE_COMMAND}
                        -DCMD1=${CMD1}
                        -DCMD2=${CMD2}
                        -P ${CMAKE_SOURCE_DIR}/examples/cmake/runcommands.cmake
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()

endfunction()

function(unittest_audio)

    # Get name of the folder we are in
    # --------------------------------

    get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    # SDL2 test
    # ---------

    set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test-sdl2.lua")
    if(NOT EXISTS ${TEST_SCRIPT})
        set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test.lua")
    endif()

    set(REF_WAV "${CMAKE_CURRENT_SOURCE_DIR}/reference-sdl2.wav")
    if(NOT EXISTS ${REF_WAV})
        set(REF_WAV "${CMAKE_CURRENT_SOURCE_DIR}/reference.wav")
    endif()

    set(CMD1 "$<TARGET_FILE:${EXECUTABLE_NAME}> --lua ${TEST_SCRIPT}")
    set(CMD2 "${CMAKE_COMMAND} -E compare_files ${REF_WAV} audio.wav")

    add_test(NAME ${EXECUTABLE_NAME}_test
        COMMAND ${CMAKE_COMMAND}
                    -DCMD1=${CMD1}
                    -DCMD2=${CMD2}
                    -P ${CMAKE_SOURCE_DIR}/examples/cmake/runcommands.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    # Emulator test
    # -------------

    if(BUILD_GBA)
        set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test-gba.lua")
        if(NOT EXISTS ${TEST_SCRIPT})
            set(TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test.lua")
        endif()

        set(REF_WAV "${CMAKE_CURRENT_SOURCE_DIR}/reference-gba.wav")
        if(NOT EXISTS ${REF_WAV})
            set(REF_WAV "${CMAKE_CURRENT_SOURCE_DIR}/reference.wav")
        endif()

        set(GBA_ROM "${CMAKE_CURRENT_SOURCE_DIR}/${EXECUTABLE_NAME}.gba")

        set(CMD1 "$<TARGET_FILE:giibiiadvance> --lua ${TEST_SCRIPT} ${GBA_ROM}")
        set(CMD2 "${CMAKE_COMMAND} -E compare_files ${REF_WAV} audio.wav")

        add_test(NAME ${EXECUTABLE_NAME}_gba_test
            COMMAND ${CMAKE_COMMAND}
                        -DCMD1=${CMD1}
                        -DCMD2=${CMD2}
                        -P ${CMAKE_SOURCE_DIR}/examples/cmake/runcommands.cmake
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()

endfunction()
