# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

function(define_example)

    # Get name of the folder we are in
    get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    add_executable(${EXECUTABLE_NAME})

    target_link_libraries(${EXECUTABLE_NAME} libgbaline)

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
endfunction()


function(unittest_screenshot)

    # Get name of the folder we are in
    get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    set(CMD1 "$<TARGET_FILE:${EXECUTABLE_NAME}> --autotest")
    set(CMD2 "$<TARGET_FILE:pngmatch> ${CMAKE_CURRENT_SOURCE_DIR}/reference.png screenshot.png")

    add_test(NAME test
        COMMAND ${CMAKE_COMMAND}
                    -DCMD1=${CMD1}
                    -DCMD2=${CMD2}
                    -P ${CMAKE_SOURCE_DIR}/examples/cmake/runcommands.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR}
    )
endfunction()
