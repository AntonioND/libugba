# SPDX-License-Identifier: GPL-3.0-only
#
# Copyright (c) 2021-2022 Antonio Niño Díaz

macro(gba_set_compiler_options elf_target)
    set(ARGS_C
        -mthumb -mthumb-interwork
        -mcpu=arm7tdmi -mtune=arm7tdmi
        -ffunction-sections -fdata-sections
    )

    target_compile_options(${elf_target} PRIVATE
        $<$<COMPILE_LANGUAGE:C>:${ARGS_C}>
    )

    target_compile_definitions(${elf_target} PRIVATE __GBA__)

    target_link_options(${elf_target} PRIVATE
        -specs=gba.specs
        -mthumb -mthumb-interwork
        -Wl,-Map,${elf_target}.map -Wl,--gc-sections
    )
endmacro()

macro(make_gba_rom elf_target rom_name game_title game_code)
    add_custom_command(
        OUTPUT ${rom_name}.gba
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${elf_target}> ${rom_name}.gba
        COMMAND $<TARGET_FILE:gbafix> ${rom_name}.gba -t${game_title} -c${game_code}
        DEPENDS $<TARGET_FILE:${elf_target}>
    )

    add_custom_target(${rom_name}_target ALL
        DEPENDS ${rom_name}.gba
    )
endmacro()
