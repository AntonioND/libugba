libugba template
================

1. Preparation
--------------

This is a template of how to integrate libugba in your own project. This
template doesn't rely on the build system of ugba, and simply integrates libugba
as a submodule.

Feel free to edit the CMakeLists.txt to find UGBA and UMOD Player in your system
and to enable the integration of graphics and data using grit and bin2c
respectively.

The variable ``LIBUGBA`` needs to be modified in the Makefile to point at the
right location of the libugba folder. The variable ``UMOD_PLAYER`` needs to
point at the ``player`` folder inside the ``umod-player`` repository. The
variable ``UMOD_PACKER`` needs to point to a built packer after building UMOD
Player.

You can add files to the ``data`` folder, graphics to the ``graphics`` folder,
and music and sfx files to the ``audio`` folder. The build system will add them
to the build automatically.

2. Build for host
-----------------

It is built the same way as the main **ugba** project. For example, in Linux, to
generate the PC binary:

.. code:: bash

    mkdir build
    cd build
    cmake ..
    make -j`nproc`

3. Build for GBA
----------------

To build the GBA binary, you need to build libugba for GBA first:

.. code:: bash

    cd path/to/libugba
    make -j`nproc`

Then, you need to build UMOD Player for GBA and for the host:

.. code:: bash

    cd path/to/umod-player/player
    make -j`nproc`

    cd path/to/umod-player
    mkdir build
    cd build
    cmake ..
    make -j`nproc`

Then you can build the template:

.. code:: bash

    cd path/to/template
    make -j`nproc`
