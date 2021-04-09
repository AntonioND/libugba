// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

#include <stdlib.h>
#include <string.h>

#include <ugba/ugba.h>

#include "sound.h"

#include "../debug_utils.h"

void SWI_RegisterRamReset(uint32_t flags)
{
    // Note: LSB of SIODATA32 is always destroyed because of a bug
    REG_SIODATA32 = REG_SIODATA32 & 0xFFFF0000;

    if (flags & SWI_RAM_RESET_EWRAM)
    {
        // Right now there is no way to know which variables are in EWRAM
        Debug_Log("%: EWRAM reset not supported", __func__);
    }
    if (flags & SWI_RAM_RESET_IWRAM)
    {
        // Right now there is no way to know which variables are in IWRAM
        Debug_Log("%: IWRAM reset not supported", __func__);
    }
    if (flags & SWI_RAM_RESET_PALETTE)
    {
        memset(MEM_PALETTE, 0, MEM_PALETTE_SIZE);
    }
    if (flags & SWI_RAM_RESET_VRAM)
    {
        memset(MEM_VRAM, 0, MEM_VRAM_SIZE);
    }
    if (flags & SWI_RAM_RESET_OAM)
    {
        memset(MEM_OAM, 0 , MEM_OAM_SIZE);
    }
    if (flags & SWI_RAM_RESET_IO_SERIAL)
    {
        REG_SIODATA32 = 0;
        REG_SIOMULTI2 = 0;
        REG_SIOMULTI3 = 0;
        REG_SIOCNT = 0;
        REG_SIOMLT_SEND = 0;

        REG_RCNT = 0;

        REG_JOYCNT = 0;
        REG_JOY_RECV = 0;
        REG_JOY_TRANS = 0;
        REG_JOYSTAT = 0;
    }
    if (flags & SWI_RAM_RESET_IO_SOUND)
    {
        REG_SOUND1CNT_L = 0;
        REG_SOUND1CNT_H = 0;
        REG_SOUND1CNT_X = 0;
        REG_SOUND2CNT_L = 0;
        REG_SOUND2CNT_H = 0;
        REG_SOUND3CNT_L = 0;
        REG_SOUND3CNT_H = 0;
        REG_SOUND3CNT_X = 0;
        REG_SOUND4CNT_L = 0;
        REG_SOUND4CNT_H = 0;
        REG_SOUNDCNT_L = 0;
        REG_SOUNDCNT_H = 0x880E;
        REG_SOUNDCNT_X = 0;

        Sound_Initialize();
    }
    if (flags & SWI_RAM_RESET_IO_OTHER)
    {
        REG_DISPCNT = 0;
        REG_GREENSWAP = 0;
        REG_DISPSTAT = 0;
        REG_VCOUNT = 0;
        REG_BG0CNT = 0;
        REG_BG1CNT = 0;
        REG_BG2CNT = 0;
        REG_BG3CNT = 0;
        REG_BG0HOFS = 0;
        REG_BG0VOFS = 0;
        REG_BG1HOFS = 0;
        REG_BG1VOFS = 0;
        REG_BG2HOFS = 0;
        REG_BG2VOFS = 0;
        REG_BG3HOFS = 0;
        REG_BG3VOFS = 0;
        REG_BG2PA = 1 << 8;
        REG_BG2PB = 0;
        REG_BG2PC = 0;
        REG_BG2PD = 1 << 8;
        REG_BG2X_L = 0;
        REG_BG2Y_L = 0;
        REG_BG3PA = 1 << 8;
        REG_BG3PB = 0;
        REG_BG3PC = 0;
        REG_BG3PD = 1 << 8;
        REG_BG3X_L = 0;
        REG_BG3Y_L = 0;
        REG_WIN0H = 0;
        REG_WIN1H = 0;
        REG_WIN0V = 0;
        REG_WIN1V = 0;
        REG_WININ = 0;
        REG_WINOUT = 0;
        REG_MOSAIC = 0;
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        REG_BLDY = 0;
        REG_DMA0SAD = 0;
        REG_DMA0DAD = 0;
        REG_DMA0CNT_L = 0;
        REG_DMA1SAD = 0;
        REG_DMA1DAD = 0;
        REG_DMA1CNT_L = 0;
        REG_DMA2SAD = 0;
        REG_DMA2DAD = 0;
        REG_DMA2CNT_L = 0;
        REG_DMA3SAD = 0;
        REG_DMA3DAD = 0;
        REG_DMA3CNT_L = 0;
        REG_TM0CNT_L = 0;
        REG_TM1CNT_L = 0;
        REG_TM2CNT_L = 0;
        REG_TM3CNT_L = 0;
        REG_KEYINPUT = 0;
        REG_KEYCNT = 0;
        REG_IE = 0;
        REG_IF = 0;
        REG_IME = 0;
    }

    REG_DISPCNT = 0x0080;
}

uint32_t SWI_GetBiosChecksum(void)
{
    // Pretend the emulated system is a regular GBA
    return SWI_CHECKSUM_GBA;
}

void SWI_CpuSet(const void *src, void *dst, uint32_t len_mode)
{
    if (len_mode & SWI_MODE_32BIT)
    {
        UMOD_Assert(((uintptr_t)src & 3) == 0);
        UMOD_Assert(((uintptr_t)dst & 3) == 0);
    }
    else
    {
        UMOD_Assert(((uintptr_t)src & 1) == 0);
        UMOD_Assert(((uintptr_t)dst & 1) == 0);
    }

    int count = len_mode & 0x001FFFFF;
    uint32_t mode = len_mode & ~0x001FFFFF;

    if (mode & SWI_MODE_32BIT)
    {
        uint32_t *src_ = (uint32_t *)((uintptr_t)src & ~3);
        uint32_t *dst_ = (uint32_t *)((uintptr_t)dst & ~3);

        if (mode & SWI_MODE_FILL)
        {
            uint32_t fill = *src_;
            while (count--)
                *dst_++ = fill;
        }
        else // Copy
        {
            while (count--)
                *dst_++ = *src_++;
        }
    }
    else // 16 bit
    {
        uint16_t *src_ = (uint16_t *)((uintptr_t)src & ~1);
        uint16_t *dst_ = (uint16_t *)((uintptr_t)dst & ~1);

        if (mode & SWI_MODE_FILL)
        {
            uint16_t fill = *src_;
            while (count--)
                *dst_++ = fill;
        }
        else // Copy
        {
            while (count--)
                *dst_++ = *src_++;
        }
    }
}

void SWI_CpuFastSet(const void *src, void *dst, uint32_t len_mode)
{
    UMOD_Assert(((uintptr_t)src & 3) == 0);
    UMOD_Assert(((uintptr_t)dst & 3) == 0);
    UMOD_Assert((len_mode & 7) == 0);

    int count = len_mode & 0x001FFFF8; // Must be a multiple of 8 words
    uint32_t mode = len_mode & ~0x001FFFFF;

    uint32_t *src_ = (uint32_t *)((uintptr_t)src & ~3);
    uint32_t *dst_ = (uint32_t *)((uintptr_t)dst & ~3);

    if (mode & SWI_MODE_FILL)
    {
        uint32_t fill = *src_;
        while (count--)
            *dst_++ = fill;
    }
    else // Copy
    {
        while (count--)
            *dst_++ = *src_++;
    }
}

void SWI_BitUnPack(const void *source, void *dest, const bit_unpack_info *info)
{
    UMOD_Assert(((uintptr_t)dest & 3) == 0);

    const uint8_t *src = source;
    uint32_t *dst = dest;

    int32_t srcsize = info->source_length;
    uint32_t dataoffset = info->data_offset & ~SWI_BITUNPACK_OFFSET_ZERO;
    int zerodataflag = info->data_offset & SWI_BITUNPACK_OFFSET_ZERO;

    // Last data read from source and position of the next chunk of bits to
    // extract from it.
    uint8_t srcdata = 0;
    int src_bitindex = 0;

    // Data to write to the destination and position of the next chunk of bits
    // to insert in it.
    uint32_t dstdata = 0;
    int dst_bitindex = 0;

    while (1)
    {
        // Value being handled
        uint32_t data = 0;

        if (src_bitindex == 0)
        {
            if (srcsize == 0)
                break;

            srcdata = *src++;
            srcsize--;
        }

        switch (info->source_width)
        {
            case 1:
                data = srcdata & 0x1;
                srcdata >>= 1;
                src_bitindex++;
                break;
            case 2:
                data = srcdata & 0x3;
                srcdata >>= 2;
                src_bitindex += 2;
                break;
            case 4:
                data = srcdata & 0xF;
                srcdata >>= 4;
                src_bitindex += 4;
                break;
            case 8:
                data = srcdata;
                src_bitindex += 8;
                break;
            default:
                Debug_Log("%s: Invalid source width: %d", __func__,
                          info->source_width);
                return;
        }

        if (src_bitindex == 8)
            src_bitindex = 0;

        // The offset is added always if data != 0, and it is added even in that
        // case if the zero data flag is set.
        if (data)
            data += dataoffset;
        else if (zerodataflag)
            data += dataoffset;

        if (dst_bitindex == 0)
            dstdata = 0;

        switch (info->dest_width)
        {
            case 1:
            case 2:
            case 4:
            case 8:
            case 16:
            case 32:
                dstdata |= data << dst_bitindex;
                dst_bitindex += info->dest_width;
                break;
            default:
                Debug_Log("%s: Invalid destination width: %d", __func__,
                          info->dest_width);
                return;
        }
        if (dst_bitindex == 32)
        {
            *dst++ = dstdata;
            dst_bitindex = 0;
            dstdata = 0;
        }
    }
}

// The only difference between LZ77UnCompReadNormalWrite8bit() and
// LZ77UnCompReadNormalWrite16bit() is the width of the writes to the
// destination. There is no difference in the emulated BIOS.
static void SWI_UncompressLZ77(const void *source, void *dest)
{
    const uint8_t *src = source;
    uint16_t *dst = dest;

    // The header is 32 bits
    uint32_t header = *(uint32_t *)src;
    src += 4;

    uint32_t compression_type = (header >> 4) & 0xF;
    if (compression_type != SWI_UNCOMP_TYPE_LZ77)
    {
        Debug_Log("%s: Invalid type: %d", __func__, compression_type);
        return;
    }

    uint32_t size = (header >> 8) & 0x00FFFFFF;

    uint8_t *buffer = malloc(size + 2);
    if (buffer == NULL)
    {
        Debug_Log("%s: Not enough memory", __func__);
        return;
    }

    uint8_t *buffertmp = buffer;
    uint32_t total = 0;
    while (size > total)
    {
        uint8_t flag = *src++;

        for (int i = 0; i < 8; i++)
        {
            if (flag & 0x80)
            {
                // Compressed - Copy N+3 Bytes from Dest-Disp-1 to Dest

                uint16_t info = ((uint16_t)*src++) << 8;
                info |= (uint16_t)*src++;

                uint32_t displacement = (info & 0x0FFF);
                int num = 3 + ((info >> 12) & 0xF);
                uint32_t offset = total - displacement - 1;
                if (offset > total) // This also checks for negative values
                {
                    Debug_Log("%s: Error while decoding", __func__);
                    free(buffer);
                    return;
                }
                while (num--)
                {
                    *buffertmp++ = ((uint8_t *)buffer)[offset++];
                    total++;
                    if (size <= total)
                        break;
                }
            }
            else
            {
                // Uncompressed - Copy 1 Byte from Source to Dest
                *buffertmp++ = *src++;
                total++;
                if (size <= total)
                    break;
            }
            flag <<= 1;
        }
    }

    // Copy to destination
    memcpy(dst, buffer, size);

    free(buffer);
}

void SWI_LZ77UnCompReadNormalWrite8bit(const void *source, void *dest)
{
    UMOD_Assert(((uintptr_t)source & 3) == 0);

    SWI_UncompressLZ77(source, dest);
}

void SWI_LZ77UnCompReadNormalWrite16bit(const void *source, void *dest)
{
    UMOD_Assert(((uintptr_t)source & 3) == 0);
    UMOD_Assert(((uintptr_t)dest & 1) == 0);

    SWI_UncompressLZ77(source, dest);
}

void SWI_HuffUnComp(const void *source, void *dest)
{
    UMOD_Assert(((uintptr_t)source & 3) == 0);
    UMOD_Assert(((uintptr_t)dest & 3) == 0);

    const uint8_t *src = source;
    uint8_t *dst = dest;

    uint32_t header = *(uint32_t *)source;
    src += 4;

    uint32_t compression_type = (header >> 4) & 0xF;
    if (compression_type != SWI_UNCOMP_TYPE_HUFFMAN)
    {
        Debug_Log("%s: Invalid type: %d", __func__, compression_type);
        return;
    }

    int chunk_size = header & 0xF; // In bits
    if ((chunk_size != 4) && (chunk_size != 8))
    {
        Debug_Log("%s(): Invalid chunk size: %d", chunk_size);
        return;
    }

    int size = (header >> 8) & 0x00FFFFFF;

    uint32_t treesize = (*src * 2) + 1;
    src++;

    const uint8_t *treetable = src;

    src += treesize; // Point to the bitstream
    uint32_t *bitstream = (uint32_t *)src;

    int total = 0;
    int bit4index = 0;

    uint32_t bits = 0;
    int bitsleft = 0;

    while (1)
    {
        const uint8_t *nodeaddr = treetable;

        while (1)
        {
            int searching = 1;

            if (bitsleft == 0)
            {
                bits = *bitstream++;
                bitsleft = 32;
            }
            int node = bits >> 31; // Get bit 31

            bits <<= 1;
            bitsleft--;

            uint8_t nodeinfo = *nodeaddr;
            if ((node == 1) && (nodeinfo & BIT(6)))
                searching = 0;
            if ((node == 0) && (nodeinfo & BIT(7)))
                searching = 0;

            uint32_t offset = ((uint32_t)(nodeinfo & 0x3F)) * 2 + 2 + node;
            nodeaddr = (const uint8_t *)(((uintptr_t)nodeaddr & ~1) + offset);

            if (searching == 0)
                break;
        }

        if (chunk_size == 8)
        {
            *dst++ = *nodeaddr;
            total++;
        }
        else // if (chunk_size == 4)
        {
            if (bit4index & 1)
            {
                *dst |= (*nodeaddr) << 4;
                dst++;
                total++;
            }
            else
            {
                *dst = *nodeaddr;
            }
            bit4index ^= 1;
        }

        if (total >= size)
            break;
    }
}

static void GBA_SWI_RLUnComp(const void *source, void *dest)
{
    const uint8_t *src = source;
    uint8_t *dst = dest;

    uint32_t header = *(uint32_t *)source;

    src += 4;

    uint32_t compression_type = (header >> 4) & 0xF;
    if (compression_type != SWI_UNCOMP_TYPE_RL)
    {
        Debug_Log("%s: Invalid type: %d", __func__, compression_type);
        return;
    }

    int32_t size = (header >> 8) & 0x00FFFFFF;

    while (size > 0)
    {
        uint8_t flag = *src++;

        if (flag & BIT(7)) // Compressed - 1 byte repeated N times
        {
            int32_t len = (flag & 0x7F) + 3;
            uint8_t data = *src++;

            while (len)
            {
                *dst++ = data;
                size--;
                len--;
            }
        }
        else // N uncompressed bytes
        {
            int32_t len = (flag & 0x7F) + 1;

            while (len)
            {
                *dst++ = *src++;
                size--;
                len--;
            }
        }
    }
}

void SWI_RLUnCompWram(const void *source, void *dest)
{
    UMOD_Assert(((uintptr_t)source & 3) == 0);

    GBA_SWI_RLUnComp(source, dest);
}

void SWI_RLUnCompVram(const void *source, void *dest)
{
    UMOD_Assert(((uintptr_t)source & 3) == 0);
    UMOD_Assert(((uintptr_t)dest & 1) == 0);

    // TODO: Can the size be not a multiple of 2?

    GBA_SWI_RLUnComp(source, dest);
}

static void GBA_Diff8bitUnFilter(const void *source, void *dest)
{
    const uint8_t *src = source;
    uint8_t *dst = dest;

    uint32_t header = *(uint32_t *)source;

    src += 4;

    uint32_t compression_type = (header >> 4) & 0xF;
    if (compression_type != SWI_UNCOMP_TYPE_DIFF)
    {
        Debug_Log("%s: Invalid type: %d", __func__, compression_type);
        return;
    }

    uint32_t data_size = header & 0xF;
    if (data_size != SWI_DIFF_SIZE_8BIT)
    {
        Debug_Log("%s: Invalid data size: %d", __func__, data_size);
        return;
    }

    int32_t size = (header >> 8) & 0x00FFFFFF;

    uint8_t value = 0;

    while (size > 0)
    {
        value += *src++;
        *dst++ = value;
        size--;
    }
}

void SWI_Diff8bitUnFilterWram(const void *source, void *dest)
{
    UMOD_Assert(((uintptr_t)source & 3) == 0);

    GBA_Diff8bitUnFilter(source, dest);
}

void SWI_Diff8bitUnFilterVram(const void *source, void *dest)
{
    UMOD_Assert(((uintptr_t)source & 3) == 0);
    UMOD_Assert(((uintptr_t)dest & 1) == 0);

    // TODO: Can the size be not a multiple of 2?

    GBA_Diff8bitUnFilter(source, dest);
}

void SWI_Diff16bitUnFilter(const void *source, void *dest)
{
    UMOD_Assert(((uintptr_t)source & 3) == 0);
    UMOD_Assert(((uintptr_t)dest & 1) == 0);

    const uint16_t *src = source;
    uint16_t *dst = dest;

    uint32_t header = *(uint32_t *)source;

    src += 2;

    uint32_t compression_type = (header >> 4) & 0xF;
    if (compression_type != SWI_UNCOMP_TYPE_DIFF)
    {
        Debug_Log("%s: Invalid type: %d", __func__, compression_type);
        return;
    }

    uint32_t data_size = header & 0xF;
    if (data_size != SWI_DIFF_SIZE_16BIT)
    {
        Debug_Log("%s: Invalid data size: %d", __func__, data_size);
        return;
    }

    int32_t size = (header >> 8) & 0x00FFFFFF;

    uint16_t value = 0;

    while (size > 0)
    {
        value += *src++;
        *dst++ = value;
        size -= 2;
    }
}
