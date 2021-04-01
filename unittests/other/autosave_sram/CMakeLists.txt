# SPDX-License-Identifier: MIT
#
# Copyright (c) 2021 Antonio Niño Díaz

# This test only makes sense in the SDL2 port because there is no autosave every
# N seconds on a real GBA, SRAM is simply preserved with the battery of the
# cartridge.
set(BUILD_GBA_INTERNAL FALSE)

define_example()
unittest_sram()
