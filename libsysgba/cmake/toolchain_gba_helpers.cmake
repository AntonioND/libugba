# SPDX-License-Identifier: GPL-3.0-only
#
# Copyright (c) 2022 Antonio Niño Díaz

macro(libsysgba_set_compiler_options elf_target)
    target_link_options(${elf_target} PRIVATE
        -specs=nano.specs
        -T ${LIBSYSGBA_PATH}/source/gba_cart.ld
        -Wl,--start-group
    )
endmacro()
