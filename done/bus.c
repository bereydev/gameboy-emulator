/**
 * @file bus.c
 * @brief Game Boy Bus Emulator
 *
 * @author C la vie
 * @date 2020
 */

#include "bus.h"
#include "error.h"
#include "component.h"
#include "bit.h"
#include <inttypes.h>

int bus_remap(bus_t bus, component_t *c, addr_t offset)
{
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c->mem);
    M_REQUIRE(c->end >= c->start, ERR_ADDRESS, "input component has a start adress 0x%" PRIX16 " biger than end address 0x%" PRIX16, c->start, c->end);
    M_REQUIRE((size_t)(c->end - c->start + offset) <= c->mem->size, ERR_ADDRESS, "input offset 0x%" PRIX16 " is incorrect", offset);

    size_t s = (size_t)(c->end - c->start + 1);
    for (size_t i = 0; i < s; ++i)
    {
        bus[c->start + i] = &c->mem->memory[offset + i];
    }

    return ERR_NONE;
}

int bus_forced_plug(bus_t bus, component_t *c, addr_t start, addr_t end, addr_t offset)
{
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c->mem);
    M_REQUIRE(end >= start, ERR_BAD_PARAMETER, "inputs start(%x) is greater or equal to end(%x)", start, end);
    M_REQUIRE((size_t)(end - start + offset) <= c->mem->size, ERR_ADDRESS, "input offset (%x) is incorrect", offset);
    c->start = start;
    c->end = end;
    if (bus_remap(bus, c, offset) != ERR_NONE)
    {
        c->start = 0;
        c->end = 0;
        return ERR_BAD_PARAMETER;
    }

    return ERR_NONE;
}

int bus_plug(bus_t bus, component_t *c, addr_t start, addr_t end)
{
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(c);

    for (size_t i = start; i <= end; ++i)
    {
        if (bus[i] != NULL)
            return ERR_ADDRESS;
    }
    return bus_forced_plug(bus, c, start, end, 0);
}

int bus_unplug(bus_t bus, component_t *c)
{
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(c);

    for (size_t i = c->start; i <= c->end; ++i)
    {
        bus[i] = NULL;
    }
    c->start = 0;
    c->end = 0;

    return ERR_NONE;
}

int bus_read(const bus_t bus, addr_t address, data_t *data)
{
    M_REQUIRE_NON_NULL(data);
    M_REQUIRE_NON_NULL(bus);

    *data = (bus[address] == NULL) ? 0xFF : *bus[address];

    return ERR_NONE;
}

int bus_write(bus_t bus, addr_t address, data_t data)
{
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(bus[address]);

    *bus[address] = data;

    return ERR_NONE;
}

int bus_read16(const bus_t bus, addr_t address, addr_t *data16)
{
    M_REQUIRE_NON_NULL(data16);
    M_REQUIRE_NON_NULL(bus);
    // on récupère les deux octets d'un coup en castant la valeur pointée par address
    // 0xFFFF étant la dernière addresse du bus il n'est pas possible d'y lire 2 octets
    // sans causer de segmentation fault
    *data16 = (bus[address] == NULL || address == 0xFFFF) ? 0xFF : *((addr_t *)bus[address]);

    return ERR_NONE;
}

int bus_write16(bus_t bus, addr_t address, addr_t data16)
{
    M_REQUIRE_NON_NULL(bus);
    *((addr_t *)bus[address]) = data16;

    return ERR_NONE;
}
