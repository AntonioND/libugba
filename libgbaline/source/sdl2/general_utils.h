// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2020 Antonio Niño Díaz

#ifndef SDL2_GENERAL_UTILS_H__
#define SDL2_GENERAL_UTILS_H__

#include <stdint.h>

// TODO: Remove this
typedef uint64_t u64;
typedef int64_t s64;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint8_t u8;
typedef int8_t s8;

#define BIT(n) (1 << (n))

#if defined(_MSC_VER)
# define unused__
#else
# define unused__ __attribute__((unused))
#endif

#if defined(_MSC_VER)
# define ALIGNED(x) __declspec(align(x))
#else
# define ALIGNED(x) __attribute__((aligned(x)))
#endif

#endif // SDL2_GENERAL_UTILS__
