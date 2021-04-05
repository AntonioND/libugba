Library
=======

SDL2
----

- Add helper to configure user input.
- Interrupts: SERIAL and GAMEPAK are missing.
- DMA 3 Video Capture Mode
- Screenshots.

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

- Mosaic effect is broken, it doesn't work like on hardware.
