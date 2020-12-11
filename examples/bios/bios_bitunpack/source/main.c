// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Test that uses BitUnPack in several ways.

#include <string.h>

#include <ugba/ugba.h>

// 4 K buffer aligned to 32 bit
static uint32_t buffer[(4 * 1024) / sizeof(uint32_t)];

static void verify(const void *reference, size_t size)
{
    if (memcmp(&(buffer[0]), reference, size) == 0)
        CON_Print("OK ");
    else
        CON_Print("-- ");
}

// ----------------------------------------------------------------------------

const uint8_t src_1b[4] = {
    0xFE,
    0xB7,
    0xA5,
    0xC3,
};

const uint8_t dst_1b_1b[4] = {
    0xFE,
    0xB7,
    0xA5,
    0xC3,
};

const uint8_t dst_1b_2b[8] = {
    0x54, 0x55,
    0x15, 0x45,
    0x11, 0x44,
    0x05, 0x50,
};

const uint8_t dst_1b_4b[16] = {
    0x10, 0x11, 0x11, 0x11,
    0x11, 0x01, 0x11, 0x10,
    0x01, 0x01, 0x10, 0x10,
    0x11, 0x00, 0x00, 0x11,
};

const uint8_t dst_1b_8b[32] = {
    0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 0, 1, 1, 0, 1,
    1, 0, 1, 0, 0, 1, 0, 1,
    1, 1, 0, 0, 0, 0, 1, 1,
};

const uint16_t dst_1b_16b[32] = {
    0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 0, 1, 1, 0, 1,
    1, 0, 1, 0, 0, 1, 0, 1,
    1, 1, 0, 0, 0, 0, 1, 1,
};

const uint32_t dst_1b_32b[32] = {
    0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 0, 1, 1, 0, 1,
    1, 0, 1, 0, 0, 1, 0, 1,
    1, 1, 0, 0, 0, 0, 1, 1,
};

void test_1b(void)
{
    bit_unpack_info info;

    info.source_length = 4;
    info.source_width = 1;
    info.data_offset = 0;

    const void *src = &src_1b[0];
    void *dest = &buffer[0];

    CON_Print("1B: ");

    info.dest_width = 1;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_1b_1b[0], sizeof(dst_1b_1b));

    info.dest_width = 2;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_1b_2b[0], sizeof(dst_1b_2b));

    info.dest_width = 4;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_1b_4b[0], sizeof(dst_1b_4b));

    info.dest_width = 8;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_1b_8b[0], sizeof(dst_1b_8b));

    info.dest_width = 16;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_1b_16b[0], sizeof(dst_1b_16b));

    info.dest_width = 32;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_1b_32b[0], sizeof(dst_1b_32b));

    CON_Print("\n");
}

// ----------------------------------------------------------------------------

const uint8_t src_2b[4] = {
    0xFE,
    0xB7,
    0xA5,
    0xC3,
};

const uint8_t dst_2b_2b[4] = {
    0xFE,
    0xB7,
    0xA5,
    0xC3,
};

const uint8_t dst_2b_4b[8] = {
    0x32, 0x33,
    0x13, 0x23,
    0x11, 0x22,
    0x03, 0x30,
};

const uint8_t dst_2b_8b[16] = {
    2, 3, 3, 3,
    3, 1, 3, 2,
    1, 1, 2, 2,
    3, 0, 0, 3,
};

const uint16_t dst_2b_16b[16] = {
    2, 3, 3, 3,
    3, 1, 3, 2,
    1, 1, 2, 2,
    3, 0, 0, 3,
};

const uint32_t dst_2b_32b[16] = {
    2, 3, 3, 3,
    3, 1, 3, 2,
    1, 1, 2, 2,
    3, 0, 0, 3,
};

void test_2b(void)
{
    bit_unpack_info info;

    info.source_length = 4;
    info.source_width = 2;
    info.data_offset = 0;

    const void *src = &src_2b[0];
    void *dest = &buffer[0];

    CON_Print("2B: ");

    CON_Print("   ");

    info.dest_width = 2;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_2b_2b[0], sizeof(dst_2b_2b));

    info.dest_width = 4;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_2b_4b[0], sizeof(dst_2b_4b));

    info.dest_width = 8;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_2b_8b[0], sizeof(dst_2b_8b));

    info.dest_width = 16;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_2b_16b[0], sizeof(dst_2b_16b));

    info.dest_width = 32;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_2b_32b[0], sizeof(dst_2b_32b));

    CON_Print("\n");
}

// ----------------------------------------------------------------------------

const uint8_t src_4b[4] = {
    0xFE,
    0xB7,
    0xA5,
    0xC3,
};

const uint8_t dst_4b_4b[4] = {
    0xFE,
    0xB7,
    0xA5,
    0xC3,
};

const uint8_t dst_4b_8b[8] = {
    0xE, 0xF,
    0x7, 0xB,
    0x5, 0xA,
    0x3, 0xC,
};

const uint16_t dst_4b_16b[8] = {
    0xE, 0xF,
    0x7, 0xB,
    0x5, 0xA,
    0x3, 0xC,
};

const uint32_t dst_4b_32b[8] = {
    0xE, 0xF,
    0x7, 0xB,
    0x5, 0xA,
    0x3, 0xC,
};

void test_4b(void)
{
    bit_unpack_info info;

    info.source_length = 4;
    info.source_width = 4;
    info.data_offset = 0;

    const void *src = &src_2b[0];
    void *dest = &buffer[0];

    CON_Print("4B: ");

    CON_Print("      ");

    info.dest_width = 4;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_4b_4b[0], sizeof(dst_4b_4b));

    info.dest_width = 8;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_4b_8b[0], sizeof(dst_4b_8b));

    info.dest_width = 16;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_4b_16b[0], sizeof(dst_4b_16b));

    info.dest_width = 32;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_4b_32b[0], sizeof(dst_4b_32b));

    CON_Print("\n");
}

// ----------------------------------------------------------------------------

const uint8_t src_8b[4] = {
    0xFE,
    0xB7,
    0xA5,
    0xC3,
};

const uint8_t dst_8b_8b[4] = {
    0xFE,
    0xB7,
    0xA5,
    0xC3,
};

const uint16_t dst_8b_16b[4] = {
    0xFE,
    0xB7,
    0xA5,
    0xC3,
};

const uint32_t dst_8b_32b[4] = {
    0xFE,
    0xB7,
    0xA5,
    0xC3,
};

void test_8b(void)
{
    bit_unpack_info info;

    info.source_length = 4;
    info.source_width = 8;
    info.data_offset = 0;

    const void *src = &src_2b[0];
    void *dest = &buffer[0];

    CON_Print("8B: ");

    CON_Print("         ");

    info.dest_width = 8;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_8b_8b[0], sizeof(dst_8b_8b));

    info.dest_width = 16;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_8b_16b[0], sizeof(dst_8b_16b));

    info.dest_width = 32;
    SWI_BitUnPack(src, dest, &info);
    verify(&dst_8b_32b[0], sizeof(dst_8b_32b));

    CON_Print("\n");
}

// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    REG_DISPCNT = DISPCNT_BG_MODE(0);

    CON_InitDefault();

    test_1b();
    test_2b();
    test_4b();
    test_8b();

    while (1)
        SWI_VBlankIntrWait();

    return 0;
}
