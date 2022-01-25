# SPDX-License-Identifier: GPL-3.0-only
#
# Copyright (c) 2021-2022 Antonio Niño Díaz

macro(ugba_search_toolchain_gba found_toolchain)

    if(USE_DEVKITARM)
        # If the user wants to use devkitARM, hardcode the path
        set(ARM_GCC_PATH "/opt/devkitpro/devkitARM/bin/")
    else()
        # If not, look for the toolchain in common locations
        find_program(ARM_GCC_PATH_find arm-none-eabi-gcc PATHS "" "/usr/bin")

        # Remove the name of the binary, and append a slash
        get_filename_component(ARM_GCC_PATH_dir ${ARM_GCC_PATH_find} DIRECTORY)
        set(ARM_GCC_PATH "${ARM_GCC_PATH_dir}/")
    endif()

    # Check if the toolchain exists
    if(EXISTS "${ARM_GCC_PATH}arm-none-eabi-gcc")
        message(STATUS "GBA toolchain found: ${ARM_GCC_PATH}")
        set(${found_toolchain} ON)
    else()
        message(STATUS "GBA toolchain not found")
        set(${found_toolchain} OFF)
    endif()
endmacro()


macro(ugba_toolchain_gba)
    set(CMAKE_ASM_COMPILER "${ARM_GCC_PATH}arm-none-eabi-gcc")
    set(CMAKE_C_COMPILER "${ARM_GCC_PATH}arm-none-eabi-gcc")
    set(CMAKE_CXX_COMPILER "${ARM_GCC_PATH}arm-none-eabi-g++")
    set(CMAKE_LINKER "${ARM_GCC_PATH}arm-none-eabi-ld")
    set(CMAKE_AR "${ARM_GCC_PATH}arm-none-eabi-ar")
    set(CMAKE_OBJCOPY "${ARM_GCC_PATH}arm-none-eabi-objcopy")

    set(CMAKE_SYSTEM_NAME "Game Boy Advance")
    set(CMAKE_SYSTEM_PROCESSOR ARM)

    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

    set(CMAKE_ASM_FLAGS "-x assembler-with-cpp")
endmacro()
