#include "cpu-storage.h"
#include "util.h"
#include <stdio.h>
int main(void) {
    cpu_t cpu;
    size_t size = 255;
    bus_t bus = {0};
    component_t c = {NULL, 0, 0};
    do{
        component_create(&c, size);
        cpu_init(&cpu);
        cpu_plug(&cpu,&bus);
        bus_forced_plug(bus, &c, 0, (addr_t)(size-1), 0);
    } while(0);
    for (size_t i = 0; i < size; ++i) {
        cpu_write_at_idx(&cpu, (addr_t)i, (data_t)i);
    }
}

