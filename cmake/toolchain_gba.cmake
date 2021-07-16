# SPDX-License-Identifier: GPL-3.0-only
#
# Copyright (c) 2021 Antonio Niño Díaz

if(DEFINED ENV{DEVKITARM})
    set(DEVKITARM_BIN "$ENV{DEVKITARM}/bin/")
else()
    set(DEVKITARM_BIN "/opt/devkitpro/devkitARM/bin/")
endif()

set(CMAKE_ASM_COMPILER "${DEVKITARM_BIN}arm-none-eabi-gcc")
set(CMAKE_C_COMPILER "${DEVKITARM_BIN}arm-none-eabi-gcc")
set(CMAKE_OBJCOPY "${DEVKITARM_BIN}arm-none-eabi-objcopy")
set(CMAKE_LINKER "${DEVKITARM_BIN}arm-none-eabi-ld")
set(CMAKE_AR "${DEVKITARM_BIN}arm-none-eabi-ar")

set(CMAKE_SYSTEM_NAME "Game Boy Advance")
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(WARNFLAGS "-Wall -Wextra -Wno-unused-parameter")
set(ARCHFLAGS "-mthumb -mthumb-interwork")
set(DEFINES "-D__GBA__")

set(CMAKE_ASM_FLAGS "${ARCHFLAGS} ${DEFINES} -c -x assembler-with-cpp")
set(CMAKE_C_FLAGS "${ARCHFLAGS} ${DEFINES} ${WARNFLAGS} -g -O3 \
    -mcpu=arm7tdmi -mtune=arm7tdmi -ffunction-sections -fdata-sections")
