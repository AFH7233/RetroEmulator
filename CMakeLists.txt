cmake_minimum_required(VERSION 3.27)
project(retro_emulator C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -DDEBUG")

add_executable(retro_emulator emulator.c
        lib/register.h
        lib/embedded_system.h
        lib/embedded_system.c
        lib/device_manager.h
        lib/device_manager.c
        lib/memory.h
        lib/memory.c
        lib/opcodes.h
        lib/log.h)
