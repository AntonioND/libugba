// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Test that makes sure that the sine implementation in the library has an error
// lower than the expected one.

#include <math.h>
#include <stdio.h>

#include <ugba/ugba.h>

#define MAX_ABS_ERROR   (27)

int32_t sin_approx(int32_t x)
{
    return FP_Sin(x);
}

int32_t sin_real(int32_t x)
{
    return 0x10000 * sin(x * 2 * M_PI / 0x10000);
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

        if ((error < -MAX_ABS_ERROR) || (MAX_ABS_ERROR < error))
        {
            printf("%d: Approx = %08X | Real = %08X | Error = %d\n",
                   x, approx, real, error);
            failed = 1;
        }
    }

    return failed;
}
