# SPDX-License-Identifier: MIT
#
# Copyright (c) 2021 Antonio Niño Díaz

define_example()
unittest_screenshot()

# Copy saved data file to the same directory where the binary is.
configure_file(save_sram.sav save_sram.sav COPYONLY)
