cmake_minimum_required(VERSION 3.15)
project(done C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(done
        alu.c
        alu.h
        bit.c
        bit.h
        brouillon.c
        bus.c
        bus.h
        component.c
        component.h
        error.c
        error.h
        gameboy.h
        gameboy.c
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

add_executable(unit-test-alu unit-test-alu.c)
