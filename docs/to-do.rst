Library
=======

SDL2
----

- Allow setting up screen size.
- Add helper to configure user input.
- Interrupts: SERIAL and GAMEPAK are missing.
- DMA 3 Video Capture Mode
- Save SRAM to file every few seconds?

Common
------

- Definitions for serial registers.
- "Infinite" maps with finite tileset.
- "Infinite" maps with "infinite" tileset.
- DLDI.
- libgba compatibility.

Examples
========

- DMA 3 Video Capture Mode
- Make sound examples run with CTest.

Unit tests
==========

- BIOS services (corner cases in decompress functions).

General
=======

- Public interface cleanup.
- Documentation.
- Turn some examples into unit tests, and add more unit tests.

Known bugs
==========

- The PSG sound channels aren't too accurate.
- Mosaic effect is broken, it doesn't work like on hardware.
