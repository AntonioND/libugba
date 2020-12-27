// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Test that makes sure that the sine and cosine implementations in the library
// have an error lower than the expected one.

#include <math.h>
#include <stdio.h>

#include <ugba/ugba.h>

#define MAX_ABS_ERROR   (13)

int32_t sin_approx(int32_t x)
{
    return FP_Sin(x);
}

int32_t sin_real(int32_t x)
{
    double angle = x;

    double result = 65536.0 * sin((angle * 2.0 * M_PI) / 65536.0);

    // Round instead of clamping
    if (result > 0.0)
        result += 0.5;
    else
        result -= 0.5;

    return result;
}

int32_t cos_approx(int32_t x)
{
    return FP_Cos(x);
}

int32_t cos_real(int32_t x)
{
    double angle = x;

    double result = 65536.0 * cos((angle * 2.0 * M_PI) / 65536.0);

    // Round instead of clamping
    if (result > 0.0)
        result += 0.5;
    else
        result -= 0.5;

    return result;
}

int main(int argc, char *argv[])
{
    UGBA_InitHeadless(&argc, &argv);

    int failed = 0;

    // Test 3 full circles
    for (int32_t x = -FP_2_PI; x < 2 * FP_2_PI; x++)
    {
        int32_t approx = sin_approx(x);
        int32_t real = sin_real(x);
        int32_t error = approx - real;

        // For x = 0, pi/2, pi, 3*pi/2, 2*pi, etc,  make sure that both
        // functions give the same result.
        int allowed_error;
        if ((x & (FP_PI_2 - 1)) == 0)
            allowed_error = 0;
        else
            allowed_error = MAX_ABS_ERROR;

        if ((error < -allowed_error) || (allowed_error < error))
        {
            printf("sin(%X): Approx = %08X | Real = %08X | Error = %d\n",
                   x, approx, real, error);
            failed = 1;
        }
    }

    // Test 3 full circles
    for (int32_t x = -FP_2_PI; x < 2 * FP_2_PI; x++)
    {
        int32_t approx = cos_approx(x);
        int32_t real = cos_real(x);
        int32_t error = approx - real;

        // For x = 0, pi/2, pi, 3*pi/2, 2*pi, etc,  make sure that both
        // functions give the same result.
        int allowed_error;
        if ((x & (FP_PI_2 - 1)) == 0)
            allowed_error = 0;
        else
            allowed_error = MAX_ABS_ERROR;

        if ((error < -allowed_error) || (allowed_error < error))
        {
            printf("cos(%X): Approx = %08X | Real = %08X | Error = %d\n",
                   x, approx, real, error);
            failed = 1;
        }
    }

    return failed;
}
