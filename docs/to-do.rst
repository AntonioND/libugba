Library
=======

SDL2
----

- Interrupts: SERIAL and GAMEPAK are missing.
- DMA 3 Video Capture Mode

Common
------

- Definitions for serial registers.
- "Infinite" maps with finite tileset.
- "Infinite" maps with "infinite" tileset.
- DLDI.
- libgba compatibility.
- Improve build system.

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

- Writing to OAM using the provided definitions may not work correctly. GCC
  sometimes (in Debug builds, apparently) generates byte writes to OAM when
  modifying 16-bit fields. This GCC bug seems to not be fixed, and it prevents
  a trivial fix of just flagging all fields as volatile:
  https://gcc.gnu.org/bugzilla/show_bug.cgi?id=50521
