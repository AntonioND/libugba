# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020-2022 Antonio Niño Díaz

ugba_toolchain_sdl2()

add_executable(bin2c bin2c.c)

compiler_flags_sdl2(bin2c)
linker_flags_sdl2(bin2c)
