# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Antonio Niño Díaz

cmake_minimum_required(VERSION 3.15)

macro(exec_check command)
    message(VERBOSE "${command}")

    # This macro gets a string as an argument. It is needed to split it into
    # program and arguments for execute_process()
    string(REPLACE " " ";" CMD_LIST ${command})

    execute_process(COMMAND ${CMD_LIST} RESULT_VARIABLE RESULT_CODE)

    if(RESULT_CODE)
        message(FATAL_ERROR "Error: ${command}")
    endif()
endmacro()

exec_check(${CMD1})
exec_check(${CMD2})
