Library
-------

- Interrupts: SERIAL, DMA0-3 and GAMEPAK are missing.
- Examples for interrupts.
- Examples for graphics (windows, blending, etc)
- DMA (sound, special modes).
- Examples for DMA (non-repeated copies in HBL and VBL modes).
- BIOS services.
- Examples for BIOS services.

Extended library functionality
------------------------------

- Nested IRQ handling.
- "Infinite" maps with finite tileset.
- "Infinite" maps with "infinite" tileset.
- DLDI.

SDL2 port
---------

- Add debugger windows from GiiBiiAdvance.
- Add helper to configure user input.
- Add build option to remove the Lua interpreter and screenshot functionality in
  release builds.

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
