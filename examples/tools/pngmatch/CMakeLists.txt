# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

add_executable(pngmatch pngmatch.c)

# libpng is required

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(pngmatch PRIVATE png)
else()
    target_link_libraries(pngmatch PRIVATE ${PNG_LIBRARIES})
    target_include_directories(pngmatch PRIVATE ${PNG_INCLUDE_DIRS})
endif()
