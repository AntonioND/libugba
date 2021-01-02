# SPDX-License-Identifier: MIT
#
# Copyright (c) 2021 Antonio Niño Díaz

# This test doesn't work in the SDL2 port because there is no IWRAM, or EWRAM,
# and code can't be built as ARM or Thumb either.

# It is simply built to make sure that the SDL2 port ignores the defines used to
# place code in different locations works, as well as the macros to force code
# to be compiled as ARM or Thumb.

define_example()
#unittest_screenshot()
