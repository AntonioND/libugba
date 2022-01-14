# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020-2022 Antonio Niño Díaz

ugba_toolchain_sdl2()

add_executable(gritfix gritfix.c)

compiler_flags_sdl2(gritfix)
linker_flags_sdl2(gritfix)
