// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <ugba/ugba.h>

#ifndef M_PI
# define M_PI    (3.1415926535897932384626433832795)
#endif

// Input: A full cirle is 0x10000 (PI = 0x8000)
// Output: Between 1 << 16 and -1 << 16 (1.0 to -1.0)
ARM_CODE IWRAM_CODE int32_t FP_Sin(int32_t x)
{
    // Inspired by: https://www.coranac.com/2009/07/sines/

    // Use symmetry to clamp to -pi/2 to +pi/2 (-0x4000 to 0x4000)

    x &= FP_2_PI - 1;

    if (((x & 0x4000) << 1) ^ (x & 0x8000)) // pi/2 to 3*pi/2
        x = FP_PI - x;
    else if ((x & 0xC000) == 0xC000) // 3*pi/2 to 2*pi
        x = x - FP_2_PI;

    // Calculate result

    const int64_t A = (2.0 * M_PI) * (1 << 24); // 8.24
    const int64_t B = (2.0 * M_PI - 5.0) * (1 << 24); // 8.24
    const int64_t C = (M_PI - 3.0) * (1 << 24); // 0.24

    int64_t X2 = x; // 16.0
    X2 = X2 * X2; // 32.0

    int64_t T1 = X2 * C; // 32.24
    T1 >>= 28; // 4.24

    int64_t T2 = B - T1; // 8.24
    T2 *= X2; // 40.24
    T2 >>= 27; // 13.24

    int64_t result = A - T2; // 13.24
    result *= x; // 29.24
    result >>= 24; // 29.0

    return result;
}

ARM_CODE IWRAM_CODE int32_t FP_Cos(int32_t x)
{
    return FP_Sin(x + FP_PI_2);
}
