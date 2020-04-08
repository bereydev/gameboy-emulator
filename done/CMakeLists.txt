cmake_minimum_required(VERSION 3.15)
project(done C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(done
        alu.c
        alu.h
        bit.c
        bit.h
        bus.c
        bus.h
        component.c
        component.h
        error.c
        error.h
        gameboy.h
        unit-test-cpu.c
        opcode.c
        opcode.h
        cpu-alu.c
        cpu-alu.h
        cpu-registers.h
        cpu-registers.c
        cpu-storage.c
        cpu-storage.h
        gameboy.c
        cpu.c
        cpu.h
        libsid_demo.c
        memory.c
        memory.h
        sidlib.c
        sidlib.h
        tests.h
        unit-test-alu.c
        unit-test-bit.c
        unit-test-bus.c
        unit-test-component.c
        unit-test-memory.c
        util.c
        util.h)
