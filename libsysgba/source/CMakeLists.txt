# SPDX-License-Identifier: MIT
#
# Copyright (c) 2021-2022 Antonio Niño Díaz

# Define toolchain
# ----------------

toolchain_gba()

# Define library target
# ---------------------

add_library(${LIBRARY_NAME}_gba STATIC)

# Source code, include directories and global definitions
# -------------------------------------------------------

file(GLOB SOURCES CONFIGURE_DEPENDS *.c *.h *.s)

target_sources(${LIBRARY_NAME}_gba PRIVATE ${SOURCES})

# Build options
# -------------

set(ARGS_ASM
    -mthumb -mthumb-interwork
    -ffunction-sections -fdata-sections
)

set(ARGS_C
    -Wall -Wextra -Wno-unused-parameter
    -mthumb -mthumb-interwork
    -mcpu=arm7tdmi -mtune=arm7tdmi
    -ffunction-sections -fdata-sections
)

target_compile_options(${LIBRARY_NAME}_gba PRIVATE
    $<$<COMPILE_LANGUAGE:ASM>:${ARGS_ASM}>
    $<$<COMPILE_LANGUAGE:C>:${ARGS_C}>
)
