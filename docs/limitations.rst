UGBA limitations
================

1. Sound support is limited.

   The only valid way of using the DMA streaming channels is to have a double
   sound buffer. Then, it is needed to set up a timer and start a DMA copy in
   sound streaming mode, and to time it so that the VBL interrupt handler does
   the buffer switch. It isn't possible to use timers to do the double buffer
   switch. Most games do the switch during the VBL, though, so this shouldn't be
   a problem.

   PSG channels are supported, but they are also hard to simulate in the PC
   port. In short, if you want to use one of them, set all the registers to the
   right values, then trigger the restart bit. During the VBL handler, this bit
   is checked, and the sound settings will be updated accordingly. This also
   means that the PSG flags that say if a channel is playing sound are also
   updated during the VBL handler only.

   In order to access the channel 3 wave RAM, always use the definition
   ``MEM_WAVE_RAM``, never store the pointer. This macro expands to a function
   call on PC, and points at the right wave RAM bank.

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

   Sound registers are always checked at the start of VBL, and there are no
   helpers to update them right away on PC.

4. When writing to memory or I/O registers, use the provided definitions.

   Instead of manually using addresses of VRAM or I/O registers, use the
   definitions provided by the library. On the GBA they are just numbers that
   correspond to the right address. On PC they are function calls that return
   memory regions allocated by libugba to simulate the GBA regions.

   Note that on PC there is no point in using the EWRAM, IWRAM or ROM regions,
   as the code and variables aren't located there.

5. Serial port is not supported

   I'm not sure if it will be supported in the future.

6. Interrupts can't be implemented as they are on the GBA.

   - VBLANK: It works mostly as expected. Just make sure that you call the
     function ``SWI_VBlankIntrWait()`` in your main game loop. Also, note that a
     PC has a lot more computing power than the GBA. Most likely an unoptimized
     game loop will be able to run at 60 FPS on PC no matter what you do, but it
     may be slower on a real GBA.

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
     PC port this one happens in parallel to the game logic.

     In the GBA, the CPU would switch to interrupt handling mode and return to
     the main program thread after it is done. Note that the resolution of
     timers in SDL is 1 ms, it is not possible to setup a timer that triggers an
     interrupt more frequently. Also note that the OS may get in the way and the
     interrupt handler may be called fewer times than expected.

     Additionally, all SDL timers are handled in the same thread. This means
     that a timer can't interrupt the interrupt handler of another timer. It is
     still possible to interrupt VBL or HBL handlers as they run in the main
     thread.

   - KEYPAD: On PC, the keypad state is refreshed inside
     ``SWI_VBlankIntrWait()``, so that's when the interrupt handler may be
     called.

   - DMA: Works as expected.

   - SERIAL, GAMEPAK: Not supported yet.
