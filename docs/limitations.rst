ugba limitations
================

1. There is no support for sound or the serial port.

2. It is not possible to run GBA assembly code when building for PC.

   Because of this, libraries such as Maxmod or libtonc aren't supported.

   On the SDL2 port the code is built for the architecture of the PC. Unless
   you're building the game in something like a Raspberry Pi, with support for
   the 32-bit Arm architecture, you can't build games that use assembly code.

   If you really want to write in assembly some parts of your code because you
   need to optimize it, you will need to implement the same code in C and
   compile one file or the other one depending on your target.

3. It is not possible to detect writes to I/O registers.

   When you write to some I/O registers in the GBA, some side-effects may happen
   right away, like the start of a DMA transfer, or a timer may start counting.
   This kind of side effects can easily be emulated in an emulator, but not in
   this library.

   In this library there are some helper macros to refresh the state of each
   subsystem. The helper macros do nothing on the GBA, as they aren't needed,
   but they let the simulation of the GBA know that something has happened in
   that register, and it is needed to update the internal library state, for
   example.

   All the high level helpers in the library (DMA copy functions, helpers to
   enable the timer, etc) use the macros internally, and they are safe to use.
   If you decide to write to the registers directly you'll have to use the
   macros as well.

4. When writing to memory or I/O registers, use the provided definitions.

   Instead of manually using addresses of VRAM or I/O registers, use the
   definitions provided by the library. On the GBA they (ARE NUMBERS ON PC THEY
   ARE SOMETHING ELSE)
   All your code needs to use the macros provided by the library when writing
   directly to I/O registers. On PC, they redirect to memory areas that are
   known by the hardware emulation code.

5. Interrupts can't be implemented as they are on the GBA.

   - VBLANK: It works mostly as expected. Just make sure that you call the
     function SWI_VBlankIntrWait() in your main game loop.

   - HBLANK: This one is very different on PC. On GBA, the HBLANK interrupt
     happens whenever a scanline is drawn on the screen, and it can be used to
     change the scroll of a background for special visual effects, for example.
     On PC, all the scanlines are drawn in SWI_VBlankIntrWait(). The way it
     works is that, whenever a line is drawn in the screen redraw loop, the
     HBLANK interrupt handler is called. This means that, unless your game
     relies on the timing of the interrupts, your code should work the same way
     in PC and GBA. HBLANK-triggered DMA is also taken care of in this loop.

   - VCOUNT: See the explanation of HBLANK. In this case, the interrupt handler
     is simply called when the right line is reached, before drawing the line.

   - TIMER: The interrupt handlers are passed as callbacks to the interfaces in
     SDL to create timers. This means that, unlike the other interrupts, in the
     PC port this one happens in parallel to the game logic. In the GBA, the CPU
     would switch to interrupt handling mode and return to the main program
     thread after it is done. Note that the resolution of timers in SDL is 1 ms,
     it is not possible to setup a timer that triggers an interrupt more
     frequently. Also note that the OS may get in the way and the interrupt
     handler may be called fewer times than expected.

    - KEYPAD: On PC, the keypad state is refreshed inside SWI_VBlankIntrWait(),
      so that's when the interrupt handler may be called.

    - SERIAL, DMA, GAMEPAK: Not supported yet.