Library
=======

SDL2
----

- Allow setting up screen size.
- Add helper to configure user input.
- Interrupts: SERIAL and GAMEPAK are missing.
- DMA 3 Video Capture Mode
- Save SRAM to file every few seconds?
- Fix wave RAM viewer.

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
- PSG sound channels when multiple channels are used at the same time.
- DMA + PSG sound channels at the same time.

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
- PSG channel 1 sweep is a bit broken. See the example ``basic_psg_ch1`` to see
  what fails.
- Mosaic effect is broken, it doesn't work like on hardware.
