Universal GBA Library
=====================

1. Introduction
---------------

This is a library for development of GBA games. It can be used to build actual
GBA game ROMs, but it can also target a regular PC so that it is easier to debug
the logic in the game. It is written in C.

This means you can do things like:

- Build a GBA ROM and run it in an emulator.
- Build a Linux executable file and debug it using GDB.
- Build a Windows executable with MSVC and debug it using Visual Studio.
- Easily implement unit tests for your game and run them on your PC.

It comes with several examples of how to use each one of the subsystems of the
library. All examples are also used as unit tests, and they are part of the
automated regresion testing system of the library.

Also note that most of the library is optional and is included for conveniency.
The only interfaces that you must use in your project are described in `this
document <docs/interfaces.rst>`_.

Note that this is still an early version of the library. The public interface is
likely to change a bit, so expect breaking changes at some point. Hopefully,
they will be minimal, and the interfaces described in the link above will most
likely stay the same even if the internal code changes. The SDL2 port needs a
pretty big cleanup.

The GBA version of the library is a static library licensed under the MIT
license. The PC version of the library is a dynamic library licensed under the
LGPL-3.0 license. The other license files of the repository correspond to some
resources used in the examples (like music and sound effects), but don't affect
the library itself.

This repository only holds the library code. For more information about testing
check the following repository: https://github.com/AntonioND/ugba-testing

The following repository contains an example of a project that uses **libugba**:
https://github.com/AntonioND/ugba-template

2. Limitations
--------------

The PC version of the library behaves in a different way as the GBA version of
it. Take a look at `here <docs/limitations.rst>`_ for more details.

For a list of planned features, check `this link <docs/to-do.rst>`_.

3. Dependencies
---------------

To generate PC executables:

Required
^^^^^^^^

- **GCC**, **Clang**, **MSVC** or another compiler supported by CMake.
- **CMake 3.15** or later
- **SDL2 2.0.7** or later (AudioStream support needed)

Optional
^^^^^^^^

For the SDL port:

- **libpng 1.6** or later (Simplified API support needed): Needed to save PNGs
  like screenshots and dumps from VRAM debugger windows.
- **liblua 5.2** or later (Integer support needed): Needed for the Lua
  interpreter used for the unit tests.

To generate the GBA library file:

- `devkitPro`_: You need to install devkitPro following the instructions in this
  link, then follow the instructions in the sections below:
  https://devkitpro.org/wiki/Getting_Started

To convert the font used by the library to the GBA format:

- **Grit**. It comes with devkitPro. If you don't install devkitPro because you
  only want to build the PC executables, you still need to get Grit. Note that
  you will need to add it to your system's ``PATH`` environment variable if you
  download the standalone binary: https://github.com/devkitPro/grit/releases

4. Build library
----------------

Linux
^^^^^

If you're on Linux or any Linux-like environment (like MinGW or Cygwin), install
the dependencies using your package manager. For example, in Debian or Ubuntu:

.. code:: bash

    sudo apt install libsdl2-dev liblua5.4-dev libpng-dev

Clone this repository:

.. code:: bash

    git clone https://github.com/AntonioND/libugba
    cd libugba

Finally, go to the folder of **libugba**. The following will build the library:

.. code:: bash

    mkdir build
    cd build
    cmake ..
    make

Note: In order to make the compilation process faster you can run make in
multiple threads by doing ``make -j`nproc``.

Windows
^^^^^^^

In order to build with **MinGW** or **Cygwin**, you should use the Linux
instructions. The following instructions have been tested with Microsoft Visual
C++ 2019.

You need to install `vcpkg`_. In short, open a **PowerShell** window and do:

.. code:: bash

    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    .\bootstrap-vcpkg.bat
    .\vcpkg integrate install --triplet x64-windows

Then, install the dependencies (SDL2, libpng and liblua):

.. code:: bash

    .\vcpkg install SDL2 libpng liblua --triplet x64-windows

Clone this repository:

.. code:: bash

    git clone https://github.com/AntonioND/libugba
    cd libugba

Finally, go to the folder of **ugba**. The following will build the library,
and examples, in **Developer Command Prompt for VS 2019**:

.. code:: bash

    mkdir build
    cd build
    cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\...\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
    msbuild ugba.sln

In order to get a release build, do:

.. code:: bash

    msbuild ugba.sln /property:Configuration=Release

GBA
^^^

Clone this repository and build it like this:

.. code:: bash

    git clone https://github.com/AntonioND/libugba
    cd libugba
    make

Note: In order to make the compilation process faster you can run make in
multiple threads by doing ``make -j`nproc``.

5. Acknowledgements
-------------------

- Dave Murphy (WinterMute) (and others) for devkitPro and devkitARM.
- Jasper Vijn (cearn) for Grit and Tonc.
- Martin Korth (Nocash) for no$gba and GBATEK.
- Vicki Pfau (endrift) for mGBA.

.. _devkitPro: https://devkitpro.org/
.. _vcpkg: https://github.com/microsoft/vcpkg
