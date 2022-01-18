#!/bin/bash

# Build tools

pushd tools

if [ ! -d "SuperFamiconv" ]; then
    echo "[!] SuperFamiconv code not found. Fetching..."
    rm -rf SuperFamiconv
    git clone https://github.com/Optiroc/SuperFamiconv
fi

if [ ! -f "SuperFamiconv/bin/superfamiconv" ]; then
    echo "[!] SuperFamiconv binary not found: Building..."
    pushd SuperFamiconv
    make -j`nproc`
    popd
fi

if [ ! -f "bin2c" ]; then
    echo "[!] bin2c binary not found: Building..."
    gcc -o bin2c bin2c.c -Wall -Wextra
fi

if [ ! -f "lzss" ]; then
    echo "[!] lzss binary not found: Building..."
    gcc -o lzss lzss.c -Wall -Wextra
fi

popd

# Convert graphics

SUPERFAMICONV=tools/SuperFamiconv/bin/superfamiconv
BIN2C=tools/bin2c
LZSS=tools/lzss

${SUPERFAMICONV} palette \
    --mode gba \
    --palettes 1 \
    --colors 16 \
    --color-zero 000000 \
    --in-image font.png \
    --out-data font_palette.bin \
    --verbose

${SUPERFAMICONV} tiles \
    --mode gba \
    --bpp 4 \
    --tile-width 8 --tile-height 8 \
    --max-tiles 256 \
    --in-image font.png \
    --in-palette font_palette.bin \
    --out-data font_tiles.bin \
    --no-flip --no-discard \
    --verbose

${LZSS} -evf font_tiles.bin

${BIN2C} font_palette.bin .
${BIN2C} font_tiles.bin .

exit 0
