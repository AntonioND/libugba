// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <ugba/ugba.h>

// Input: A full cirle is 0x10000 (PI = 0x8000)
// Output: Between 1 << 16 and -1 << 16 (1.0 to -1.0)
ARM_CODE IWRAM_CODE int32_t FP_Sin(int32_t x)
{
    // Note: This code needs to be compiled to ARM, not Thumb. It is also placed
    // in IWRAM for even more speed.

    // Inspired by:
    //
    //     https://www.coranac.com/2009/07/sines/
    //
    // Equations used:
    //
    //     s5(x) = a * x + b * x^3 + c * x^5
    //     isin(x) = 65536 * sin(x * 2 * pi / 65536)
    //
    //     In the transformed angles:
    //
    //         pi / 2 = 16384
    //         2 * pi = 65536
    //
    //     s5'(x) = a + b * 3 * x^2 + c * 5 *x^4
    //     isin'(x) = 2 * pi * cos(x * 2 * pi / 65536)
    //
    // Conditions:
    //
    //     isin(16384) = 65536 = s5(16384)
    //     isin'(0) = 2 * pi = s5'(16384)
    //     area(s5(x)) = area(isin(x)) for x between 0 and 16384
    //
    // Solutions:
    //
    //     a = 2 * pi
    //     b = (12 / pi - 1 - pi) / (2^25)
    //     c = (3 * (2 + pi - 16 / pi)) / (2^55)
    //
    // Result:
    //
    //     A = 2 * pi
    //     B = 12 / pi - 1 - pi
    //     C = 3 * (2 + pi - 16 / pi)
    //     s5(x) = x * A + (x^3) * B / (2^25) + (x^5) * C / (2^55)
    //
    // Modified so that shifts are smaller:
    //
    //     s5(x) = x * (A + (x^2) * (B + (x^2) * C / (2^30)) / (2^25))
    //
    // Modified so that the shifts are done by 32 and they are optimized.
    // Registers are 32-bit wide, so shifting by 32 can be done by ignoring the
    // low register. Any other value needs extra calculations:
    //
    //     (B         + (x^2) * C         / (2^30)) / (2^25)
    //     (B         + (x^2) * C * (2^2) / (2^32)) / (2^25)
    //     (B * (2^7) + (x^2) * C * (2^9) / (2^32)) / (2^32)
    //
    // New modified constants:
    //
    //     A = 2 * pi
    //     B = (2^7) * 12 / pi - 1 - pi
    //     C = (2^9) * 3 * (2 + pi - 16 / pi) // Multiplied by 4
    //     s5(x) = x * (A + (x^2) * (B + (x^2) * C / (2^32)) / (2^32))
    //
    //     T1 = (x^2 * C) >> 32
    //     T2 = (x^2 * (B + T1)) >> 32
    //     s5(x) = x * (A + T2)

    // First, use symmetry to clamp to -pi/2 to +pi/2 (-0x4000 to 0x4000)

    x &= FP_2_PI - 1;

    if (x >= (3 * FP_PI_2)) // 3*pi/2 to 2*pi
        x = x - FP_2_PI;
    else if (x > FP_PI_2) // pi/2 to 3*pi/2
        x = FP_PI - x;

    // Calculate result

    const double pi = 3.1415926535897932384626433832795;

    const int64_t A = ((2.0 * pi) * (1 << 24)); // 8.24
    const int64_t B = (1 << 7) * (12.0 / pi - 1.0 - pi) * (1 << 24); // 8.24
    const int64_t C = (1 << 9) * 3.0 * (2.0 + pi - 16.0 / pi) * (1 << 24); // 8.24

    int64_t X2 = x; // 16.0
    X2 = X2 * X2; // 32.0

    int64_t T1 = X2 * C; // 40.24
    T1 >>= 32; // 8.24

    int64_t T2 = B + T1; // 8.24
    T2 *= X2; // 40.24
    T2 >>= 32; // 8.24

    int64_t result = A + T2; // 8.24
    result *= x; // 24.24
    // Add 0.5 to round up before the final shift
    result += (1 << 24) / 2; // 25.24
    result >>= 24; // 24.0

    return result;
}

ARM_CODE IWRAM_CODE int32_t FP_Cos(int32_t x)
{
    return FP_Sin(x + FP_PI_2);
}
