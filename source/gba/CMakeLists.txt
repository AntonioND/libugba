# SPDX-License-Identifier: MIT
#
# Copyright (c) 2021-2022 Antonio Niño Díaz

ugba_toolchain_gba()

# Define library target
# ---------------------

add_library(${LIBRARY_NAME}_gba STATIC)

# Link with libsysgba if needed
# -----------------------------

# By doing this, projects that use libugba don't have to link with libsysgba
# explicitly.
if(NOT USE_DEVKITARM)
    target_link_libraries(${LIBRARY_NAME}_gba libsysgba_gba)
endif()

# Source code, include directories and global definitions
# -------------------------------------------------------

target_sources(${LIBRARY_NAME}_gba PRIVATE ${SOURCES_GBA})
target_include_directories(${LIBRARY_NAME}_gba PUBLIC ${INCLUDE_PATH})

if(ENABLE_DEBUG_CHECKS)
    target_compile_definitions(${LIBRARY_NAME}_gba PRIVATE UGBA_DEBUG)
endif()

target_compile_definitions(${LIBRARY_NAME}_gba PRIVATE __GBA__)

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
