# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

cmake_minimum_required(VERSION 3.15)

# Name this project the same way as the folder it's in
get_filename_component(EXECUTABLE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

project(${EXECUTABLE_NAME})

add_executable(${EXECUTABLE_NAME})

# Link with libugba
# -----------------

# Feel free to remove the option of the two provided that you don't need.

if(TRUE)
    # 1) This code is used in case libugba is in an arbitrary location on your
    #    hard drive.

    # Get absolute path to default libugba folder
    if("${LIBUGBA_PATH}" STREQUAL "")
        get_filename_component(LIBUGBA_PATH ../libugba ABSOLUTE)
    endif()

    if(EXISTS ${LIBUGBA_PATH})
        add_subdirectory(${LIBUGBA_PATH} libugba)
    else()
        message(FATAL_ERROR "libugba folder not found")
    endif()
else()
    # 2) This code is used in case the ugba repository is added as a submodule
    #    of your repository.

    add_subdirectory(ugba/libugba)
endif()

target_link_libraries(${EXECUTABLE_NAME} libugba)

# Add source code files
# ---------------------

# Macro that searches all the source files in the specified directory in 'dir'
# and saves them in 'var'
macro(search_source_files dir var)
    file(GLOB ${var} CONFIGURE_DEPENDS ${dir}/*.c ${dir}/*.h)
endmacro()

search_source_files(source FILES_SOURCE)

target_sources(${EXECUTABLE_NAME} PRIVATE
    ${FILES_SOURCE}
)

# Add additional data files
# -------------------------

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/graphics)
    add_grit_files(graphics ${EXECUTABLE_NAME})
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/data)
    add_data_files(data ${EXECUTABLE_NAME})
endif()
