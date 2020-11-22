// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef DEFINITIONS_H__
#define DEFINITIONS_H__

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

#endif // DEFINITIONS_H__
