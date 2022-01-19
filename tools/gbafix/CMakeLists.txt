# SPDX-License-Identifier: MIT
#
# Copyright (c) 2022 Antonio Niño Díaz

ugba_toolchain_sdl2()

add_executable(gbafix gbafix.c)

ugba_compiler_flags_sdl2(gbafix)
ugba_linker_flags_sdl2(gbafix)
