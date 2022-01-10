// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef DEFINITIONS_H__
#define DEFINITIONS_H__

#define BIT(n) (1 << (n))

// For functions with unused arguments, for example
#if defined(_MSC_VER)
# define UNUSED
#else
# define UNUSED __attribute__((unused))
#endif

#if defined(_MSC_VER)
# define ALIGNED(x) __declspec(align(x))
#else
# define ALIGNED(x) __attribute__((aligned(x)))
#endif

#if defined(_MSC_VER)
# define NORETURN __declspec(noreturn)
#else
# define NORETURN __attribute__((noreturn))
#endif

// The library is built static for GBA and shared for PC
#if defined(__GBA__)
#  define EXPORT_API
#else
# if defined(_MSC_VER)
#  define EXPORT_API __declspec(dllexport)
# else
#  define EXPORT_API __attribute__((visibility("default")))
// TODO: Is this one below needed in MinGW?
//#  define EXPORT_API __attribute__((dllexport))
# endif
#endif

#ifdef __GBA__
# define ARM_CODE   __attribute__((target("arm")))
# define THUMB_CODE __attribute__((target("thumb")))
# define IWRAM_DATA __attribute__((section(".iwram_data")))
# define EWRAM_DATA __attribute__((section(".ewram_data")))
# define EWRAM_BSS  __attribute__((section(".sbss")))
# define IWRAM_CODE __attribute__((section(".iwram_code"), long_call))
# define EWRAM_CODE __attribute__((section(".ewram_code"), long_call))
#else
# define ARM_CODE
# define THUMB_CODE
# define IWRAM_DATA
# define EWRAM_DATA
# define EWRAM_BSS
# define IWRAM_CODE
# define EWRAM_CODE
#endif

#endif // DEFINITIONS_H__
