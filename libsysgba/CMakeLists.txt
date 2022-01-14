# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020-2022 Antonio Niño Díaz

cmake_minimum_required(VERSION 3.15)
project(libsysgba)
enable_language(C ASM)

set(C_STANDARD 11)
set(C_EXTENSIONS OFF)
set(C_STANDARD_REQUIRED ON)

# Global library settings
# -----------------------

set(LIBRARY_NAME libsysgba)

# Toolchain selection macros
# --------------------------

get_filename_component(LIBSYSGBA_PATH_local "." ABSOLUTE)

set(LIBSYSGBA_PATH "${LIBSYSGBA_PATH_local}" CACHE STRING "" FORCE)

# Add utilities to work with other tools
# --------------------------------------

macro(libsysgba_set_compiler_options elf_target)

    # Disable exceptions because newlib-nano doesn't support exceptions. Also,
    # the default Makefile in devkitARM disables RTTI, so do it here as well.
    set(LIBSYSGBA_ARGS_CXX -fno-exceptions -fno-rtti)

    target_compile_options(${elf_target} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:${LIBSYSGBA_ARGS_CXX}>
    )

    target_link_options(${elf_target} PRIVATE
        -specs=nano.specs
        -T ${LIBSYSGBA_PATH}/source/gba_cart.ld
        -Wl,--start-group
    )
endmacro()

# Add source code files
# ---------------------

add_subdirectory(source)
