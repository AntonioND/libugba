Universal GBA Library v0.1.0
============================

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

The PC builds of the tests are simply run on the host by passing the program a
Lua script with instructions. The GBA builds are run on `GiiBiiAdvance`_, which
has basic support for Lua scripts as well. Ideally, it would test on other
emulators. The PC builds have some parts of the emulator inside them to simulate
the GBA hardware, and it would be a good idea to test the code against a
different emulator to have a second opinion.

Note that this is still a very early version of the library. The public
interface is likely to change. Expect breaking changes every now and then.

The GBA version of the library is a static library licensed under the MIT
license. The PC version of the library is a dynamic library licensed under the
LGPL-3.0 license.

2. Limitations
--------------

The PC version of the library behaves in a different way as the GBA version of
it. Take a look at `here <docs/limitations.rst>`_ for more details. For now, the
biggest limitation is the lack of support for sound, which is planned to be
implemented.

For a list of planned features, check `this link <docs/to-do.rst>`_.

3. Dependencies
---------------

To generate GBA ROMs:

- `devkitPro`_

To generate PC executables:

- GCC, Clang, MSVC or another compiler supported by CMake.
- CMake 3.15 or later
- SDL2
- libpng 1.6 or later
- liblua 5.2 or later

To run all unit tests:

- `GiiBiiAdvance`_: To run the GBA ROM version of the unit tests.

You need to install devkitPro following the instructions in this link, then
follow the instructions in the sections below.

https://devkitpro.org/wiki/Getting_Started

Note: If you don't install devkitPro because you only want to build the PC
executables, you still need to get Grit to convert the graphics into the right
format for the GBA: https://github.com/devkitPro/grit/releases

You'll need to make sure that CMake can find it by adding it to your system's
``PATH`` environment variable.

4. Build PC library and examples on Linux
-----------------------------------------

If you're on Linux or any Linux-like environment (like MinGW or Cygwin), install
the dependencies using your package manager. For example, in Debian or Ubuntu:

.. code:: bash

    sudo apt install libsdl2-dev liblua5.4-dev libpng-dev

Finally, go to the folder of **ugba**. The following will build the library,
examples, and run all the tests to verify it's working:

.. code:: bash

    mkdir build
    cd build
    cmake ..
    make -j`nproc`
    ctest

5. Build PC library and examples on Windows
-------------------------------------------

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

6. Build GBA library and examples
---------------------------------

First, build the library. Go to ``ugba/libugba`` and type ``make``.

Then, go to any folder with an example inside ``examples``, or go to
``template``, and type ``make`` again. That should generate a file with gba
extension, that you can run in an emulator or copy to a flashcart to run on real
hardware.

Note: In order to make the compilation process faster you can run make in
multiple threads by doing ``make -j`nproc``.

If you want to run the unit tests, you need to use the CMake build system. It is
needed to have a clone of the repository of `GiiBiiAdvance`_. By default, the
build system will look for it in the parent directory of the ugba directory.

.. code:: bash

    mkdir build
    cd build
    cmake .. -DBUILD_GBA=ON
    make -j`nproc`
    ctest

7. Credits
----------

- Dave Murphy (WinterMute) for devkitPro.
- Jasper Vijn (cearn) for Grit and Tonc.
- Martin Korth (Nocash) for no$gba and GBATEK.
- Vicki Pfau (endrift) for mGBA.

.. _GiiBiiAdvance: https://github.com/AntonioND/giibiiadvance
.. _devkitPro: https://devkitpro.org/
.. _vcpkg: https://github.com/microsoft/vcpkg
