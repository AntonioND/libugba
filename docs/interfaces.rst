Minimal library interface
=========================

This document is a list of the functions and definitions that you must use if
you want your game to be compatible with **UGBA**. All other functions and
definitions are optional, and are only included in the library for convenience.

Memory definitions
------------------

The following are used to access different memory locations of the GBA:
``MEM_BIOS_ADDR``, ``MEM_EWRAM_ADDR``, ``MEM_IWRAM_ADDR``, ``MEM_IO_ADDR``,
``MEM_PALETTE_ADDR``, ``MEM_VRAM_ADDR``, ``MEM_OAM_ADDR``, ``MEM_ROM_ADDR_WS0``,
``MEM_ROM_ADDR_WS1``, ``MEM_ROM_ADDR_WS2`` and ``MEM_SRAM_ADDR``.

This includes the PSG channel 3 wave RAM (``REG_WAVE_RAM``). Note that this
memory is banked, so it is needed to always access through the definition.
Internally this checks the active bank at every access.

Also, every definition of registers with the format ``REG_xxx``.

Special registers
-----------------

Normally, writing to the following registers has side effects. In order to
replicate their behaviour in the library, it is needed to use the macro
``UGBA_RegisterUpdatedOffset()`` after writing to the registers.

1. When modifying the following registers dynamically during the screen redraw
   (in the HBL interrupt handler, for example). It isn't needed if the values
   are static during the whole frame.

    - ``REG_BG2X_L``, ``REG_BG2X_H``, ``REG_BG2X``
    - ``REG_BG2Y_L``, ``REG_BG2Y_H``, ``REG_BG2Y``
    - ``REG_BG3X_L``, ``REG_BG3X_H``, ``REG_BG3X``
    - ``REG_BG3Y_L``, ``REG_BG3Y_H``, ``REG_BG3Y``

2. When starting a DMA transfer by writing to:

   - ``REG_DMA0CNT_H``, ``REG_DMA1CNT_H``, ``REG_DMA2CNT_H``, ``REG_DMA3CNT_H``

3. When starting a timer by writing to:

   - ``REG_TM0CNT_H``, ``REG_TM1CNT_H``, ``REG_TM2CNT_H``, ``REG_TM3CNT_H``

The way the function has to be used is:

.. code:: c
    REG_TM0CNT_H = MY_VALUE;
    UGBA_RegisterUpdatedOffset(OFFSET_TM0CNT_H);

Note that all the helper functions of UGBA already do this. Take a look at their
implementation for examples of how to use this function.

Interrupt handling
------------------

The SDL2 port emulates the behaviour of the interrupt handler included with the
GBA library. This means that it is needed to use the functions in the following
file: ``libugba/include/ugba/interrupts.h``.

This interrupt handler supports interrupt nesting. This can be done by setting
``REG_IME = 1`` inside an interrupt handler. Note that, while on GBA any
interrupt handler can be interrupted, in the SDL2 port this isn't possible. For
example, a timer may never interrupt another timer because of how SDL2 handles
timers. However, a timer may interrupt the VBLANK handler.

BIOS functions
--------------

It is needed to use the helpers in ``libugba/include/ugba/bios.h`` to use the
BIOS services. The most important function is ``SWI_VBlankIntrWait()``, which is
the main function you should use to synchronise your game. However,
``SWI_Halt()`` and ``SWI_IntrWait()`` can also be used.
