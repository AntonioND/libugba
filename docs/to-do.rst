Library
=======

SDL2
----

- Allow setting up screen size.
- Add helper to configure user input.
- Interrupts: SERIAL, DMA 0-3 and GAMEPAK are missing.
- DMA 3 Video Capture Mode
- BIOS services (IRQ-related services left).
- PSG sound channels.

GBA
---

- Set register WAITCNT to the right values depending on the device.
- Nested IRQ handling.

Common
------

- "Infinite" maps with finite tileset.
- "Infinite" maps with "infinite" tileset.
- DLDI.
- libgba compatibility.

Examples
========

- Interrupts.
- DMA 3 Video Capture Mode
- Graphics: Windows, blending, fade in/out.
- PSG sound channels.
- WAITCNT.

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
