// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <math.h>

#include <ugba/ugba.h>

#include "debug_utils.h"

uint16_t SWI_Sqrt(uint32_t value)
{
    return (uint16_t)sqrt(value);
}

int16_t SWI_ArcTan(int16_t tan)
{
    int32_t r0 = tan;

    // This is what the BIOS does... Not accurate at the bounds.
    int32_t r1 = -((r0 * r0) >> 14);
    int32_t r3 = ((((int32_t)0xA9) * r1) >> 14) + 0x390;
    r3 = ((r1 * r3) >> 14) + 0x91C;
    r3 = ((r1 * r3) >> 14) + 0xFB6;
    r3 = ((r1 * r3) >> 14) + 0x16AA;
    r3 = ((r1 * r3) >> 14) + 0x2081;
    r3 = ((r1 * r3) >> 14) + 0x3651;
    r3 = ((r1 * r3) >> 14) + 0xA259;
    int32_t result = (r0 * r3) >> 16;

    return result;

    // Emulated: Accurate always
    // r0: Tan, 16bit: 1bit sign, 1bit integral part, 14bit decimal part
    // Return: "-PI/2 < THETA < PI/2" in a range of C000h-4000h.
    // result = (int32_t)(atan(((float)(int16_t)r0) / (float)(1 << 14))
    //                         * ((float)0x4000 / M_PI_2));
}

int16_t SWI_ArcTan2(int16_t x, int16_t y)
{
    int32_t x_ = x;
    int32_t y_ = y;

    if (y_ == 0)
    {
        if (x_ < 0)
            return (int16_t)0x8000;

        return 0;
    }

    if (x_ == 0)
    {
        if (y_ < 0)
            return (int16_t)0xC000;

        return 0x4000;
    }

    if (y_ >= 0)
    {
        if (x_ >= 0)
        {
            if (x_ < y_)
                return 0x4000 - SWI_ArcTan((x_ << 14) / y_);

            return SWI_ArcTan((y_ << 14) / x_);
        }

        if (-x_ < y_)
            return 0x4000 - SWI_ArcTan((x_ << 14) / y_);

        return 0x8000 + SWI_ArcTan((y_ << 14) / x_);
    }

    if (x_ > 0)
    {
        if (x_ < -y_)
            return 0xC000 - SWI_ArcTan((x_ << 14) / y_);

        return 0x10000 + SWI_ArcTan((y_ << 14) / x_);
    }

    if (-x_ > -y_)
        return 0x8000 + SWI_ArcTan((y_ << 14) / x_);

    return 0xC000 - SWI_ArcTan((x_ << 14) / y_);

    // Emulated: Accurate always
    // float x_ = ((float)x) / (float)(1 << 14);
    // float y_ = ((float)y) / (float)(1 << 14);
    // Return: 0000h-FFFFh for 0 <= THETA < 2PI.
    // int16_t result =
    //             (int32_t)(atan2(y_, x_) * (((float)0xFFFF) / (2.0 * M_PI)));
    // return result;
}

void SWI_BgAffineSet(const bg_affine_src *src, bg_affine_dst *dst,
                     uint32_t count)
{
    while (count--)
    {
        int32_t cx = src->bgx;
        int32_t cy = src->bgy;
        float dispx = (float)src->scrx;
        float dispy = (float)src->scry;
        float sx = ((float)src->scalex) / (float)(1 << 8);
        float sy = ((float)src->scaley) / (float)(1 << 8);
        float angle = (float)2.0 * (float)M_PI
                      * (((float)(uint8_t)(src->angle >> 8)) / (float)0xFF);

        float sin_ = sin(angle);
        float cos_ = cos(angle);
        dst->pa = cos_ * (float)(1 << 8) * sx;
        dst->pb = -sin_ * (float)(1 << 8) * sx;
        dst->pc = sin_ * (float)(1 << 8) * sy;
        dst->pd = cos_ * (float)(1 << 8) * sy;

        dst->xoff = cx - (int32_t)((cos_ * sx * dispx - sin_ * sx * dispy)
                   * (float)(1 << 8));
        dst->yoff = cy - (int32_t)((sin_ * sy * dispx + cos_ * sy * dispy)
                   * (float)(1 << 8));

        src++;
        dst++;
    }
}

void SWI_ObjAffineSet(const obj_affine_src *src, void *dst,
                      uint32_t count, uint32_t increment)
{
    if (increment & 1)
    {
        Debug_Log("%s: Please, set increment to a multiple of 2", __func__);
    }

    int16_t *out = dst;

    for (uint32_t i = 0; i < count; i++)
    {
        float sx = ((float)src->sx) / (float)(1 << 8);
        float sy = ((float)src->sy) / (float)(1 << 8);
        float angle = (float)2.0 * (float)M_PI
                      * (((float)(uint8_t)(src->angle >> 8)) / (float)0xFF);
        src++;

        float sin_ = sin(angle);
        float cos_ = cos(angle);
        int16_t A = (int16_t)(cos_ * (float)(1 << 8) * sx);
        int16_t B = (int16_t)(-sin_ * (float)(1 << 8) * sx);
        int16_t C = (int16_t)(sin_ * (float)(1 << 8) * sy);
        int16_t D = (int16_t)(cos_ * (float)(1 << 8) * sy);

        *out = A;
        out += increment / 2;
        *out = B;
        out += increment / 2;
        *out = C;
        out += increment / 2;
        *out = D;
        out += increment / 2;
    }
}
