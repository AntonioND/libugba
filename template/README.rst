libugba template
================

1. Preparation
--------------

This is a template of how to integrate libugba in your own project. This
template doesn't rely on the build system of ugba, and simply integrates libugba
as a submodule.

Feel free to edit the CMakeLists.txt to find libugba in your system and to
enable the integration of graphics and data using grit and bin2c respectively.

The variable ``LIBUGBA`` needs to be modified in the Makefile to point at the
right location of the libugba folder.

You can add files to the ``data`` folder and graphics to the ``graphics``
folder, and the sample build system will use them.

2. Build on host
----------------

It is built the same way as the main **ugba** project. For example, in Linux, to
generate the PC binary:

.. code:: bash

    mkdir build
    cd build
    cmake ..
    make -j`nproc`

To build the GBA binary, you need to build libugba for GBA first:

.. code:: bash

    cd path/to/libugba
    make -j`nproc`

Then you can build the template:

.. code:: bash

    cd path/to/template
    make -j`nproc`
