# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020, 2022 Antonio Niño Díaz

macro(ugba_toolchain_sdl2)

    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        add_compile_options(
            # Force all integers to be 2's complement to prevent the compiler
            # from doing optimizations because of undefined behaviour.
            -fwrapv

            # Force usage of extern for external variables
            -fno-common
        )
    elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        add_compile_definitions(
            # Silence warnings
            _USE_MATH_DEFINES
            _CRT_SECURE_NO_WARNINGS
        )
        add_compile_options(
            # Enable parallel compilation
            /MP
        )
    endif()

endmacro()
