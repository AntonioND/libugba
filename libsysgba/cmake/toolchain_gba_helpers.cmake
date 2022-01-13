# SPDX-License-Identifier: GPL-3.0-only
#
# Copyright (c) 2022 Antonio Niño Díaz

macro(libsysgba_set_compiler_options elf_target)

    # Disable exceptions because newlib-nano doesn't support exceptions. Also,
    # the default Makefile in devkitARM disables RTTI, so do it here as well.
    set(LIBSYSGBA_ARGS_CXX -fno-exceptions -fno-rtti)

    target_compile_options(${elf_target} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:${LIBSYSGBA_ARGS_CXX}>
    )

    target_link_options(${elf_target} PRIVATE
        -specs=nano.specs
        -T ${LIBSYSGBA_PATH}/source/gba_cart.ld
        -Wl,--start-group
    )
endmacro()
