# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

add_subdirectory(tools)

include(cmake/example.cmake)

add_subdirectory(dma)
add_subdirectory(graphics)
add_subdirectory(input)
add_subdirectory(interrupts)
add_subdirectory(timers)
