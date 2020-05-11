cmake_minimum_required(VERSION 3.15)
project(done C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(unit-test-cpu unit-test-cpu.c
        tests.h
        error.h error.c
        alu.h alu.c
        bit.h bit.c
        opcode.h opcode.c
        util.h util.c
        cpu.h cpu.c
        bus.h bus.c
        memory.h memory.c
        component.h component.c
        cpu-registers.h cpu-registers.c
        cpu-storage.h cpu-storage.c
        cpu-alu.h cpu-alu.c)
target_link_libraries(unit-test-cpu ${GTK_LIBRARIES} check m rt subunit pthread)

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
        util.h main.c)

target_link_libraries(done ${GTK_LIBRARIES} check m rt subunit pthread)

add_executable(main main.c
        tests.h
        error.h error.c
        alu.h alu.c
        bit.h bit.c
        opcode.h opcode.c
        util.h util.c
        cpu.h cpu.c
        bus.h bus.c
        memory.h memory.c
        component.h component.c
        cpu-registers.h cpu-registers.c
        cpu-storage.h cpu-storage.c
        cpu-alu.h cpu-alu.c)

target_link_libraries(main ${GTK_LIBRARIES} check m rt pthread)

