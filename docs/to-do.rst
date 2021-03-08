Library
-------

- Interrupts: SERIAL, DMA 0-3 and GAMEPAK are missing.
- DMA 3 Video Capture Mode
- BIOS services (IRQ-related services left).
- Set register WAITCNT to the right values depending on the device.
- PSG sound channels.
- Interfaces for bg/spr priorities, blending and fade in/out.
- Repeat bit should be ignored in immediate DMA mode.

Examples
--------

- Interrupts.
- DMA 3 Video Capture Mode
- Graphics: Windows, blending, bg/spr priorities, fade in/out.
- PSG sound channels.
- WAITCNT.

Unit tests
----------

- BIOS services (corner cases in decompress functions).

Extended library functionality
------------------------------

- Nested IRQ handling.
- "Infinite" maps with finite tileset.
- "Infinite" maps with "infinite" tileset.
- DLDI.
- libgba support.

SDL2 port
---------

- Add helper to configure user input.

General
-------

- Public interface cleanup.
- Documentation.
- Turn some examples into unit tests, and add more unit tests.

Bugs
----

- Mosaic effect is broken, it doesn't work like on hardware.
