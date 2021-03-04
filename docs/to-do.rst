Library
-------

- Interrupts: SERIAL, DMA 0-3 and GAMEPAK are missing.
- DMA 3 Video Capture Mode
- BIOS services (IRQ-related services left).
- Set register WAITCNT to the right values depending on the device.
- PSG sound channels.
- Interfaces for bg/spr priorities, blending and fade in/out.
- Add warnings for trying to copy from ROM with DMA different than 3.
- Repeat bit is ignored in immediate DMA mode.

Examples
--------

- Interrupts.
- DMA 3 Video Capture Mode
- Graphics: Windows, blending, bg/spr priorities, fade in/out.
- BIOS services (corner cases in decompress functions).
- PSG sound channels.
- WAITCNT.
- Test to make sure HBL DMA isn't triggered during VBL.

Extended library functionality
------------------------------

- Nested IRQ handling.
- "Infinite" maps with finite tileset.
- "Infinite" maps with "infinite" tileset.
- DLDI.
- libgba support.

SDL2 port
---------

- Add debugger windows from GiiBiiAdvance.
- Add helper to configure user input.
- Add build option to remove the Lua interpreter, screenshot and wav recording
  functionality in release builds.

General
-------

- Public interface cleanup.
- Documentation.
- Turn some examples into unit tests, and add more unit tests.

Bugs
----

- ``graphics/mode4_framebuffer``: For some reason, the output of the GBA and the
  SDL2 port are different. This is likely to be a bug, as they are using the
  same rendering code.
- Mosaic effect is broken, it doesn't work like on hardware.
