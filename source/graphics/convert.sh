#!/bin/bash

# Before running this, go to the root of the repository and do:
#
#     rm -rf build && mkdir build && cd build
#     cmake ..
#     make
#
# Then, go to this directory and run this script:
#
#     cd graphics
#     bash convert.sh

grit font.png -ftc -o font
../build/tools/gritfix/gritfix font.c
