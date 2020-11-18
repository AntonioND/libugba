# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

cmake_minimum_required(VERSION 3.15)
project(toncfix)
enable_language(C)

set(C_STANDARD 11)
set(C_EXTENSIONS OFF)
set(C_STANDARD_REQUIRED ON)

add_executable(toncfix)

target_sources(toncfix PRIVATE toncfix.c)
