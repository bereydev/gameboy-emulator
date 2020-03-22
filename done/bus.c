#include "bus.h"
#include "error.h"

int bus_remap(bus_t bus, component_t* c, addr_t offset){
    //Pas sur pour le check du non NULL pour le tableau étant donné que ce n'est pas
    //vraiment un pointeur mais un expression convertie en pointeur
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE( (c->end - c->start + offset) <= c->mem.size, ERR_ADDRESS, offset);
    // l'adresse start du bus pointera sur l'adresse offset de la mémoire du composant
    bus[c->start] = &c->mem.memory[offset];
    return ERR_NONE;
}

int bus_forced_plug(bus_t bus, component_t* c, addr_t start, addr_t end, addr_t offset){
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(c);
    //TODO ERR_BAD_PARAMETER ou ERR_ADDRESS ?
    M_REQUIRE(0x0000 <= start && start <= 0xFFFF, ERR_BAD_PARAMETER, start);
    M_REQUIRE(0x0000 <= end && end <= 0xFFFF, ERR_BAD_PARAMETER, end);
    M_REQUIRE( (end - start + offset) <= c->mem.size, ERR_ADDRESS, offset);
    /*TODO dans l'énoncé ils dire de changer les valeur de end et start après bus_remap
    * mais je ne vois pas quoi passer en paramètre de bus_remap...
     */
    c->end = end;
    c->start = start;
    if (bus_remap(bus, c, offset) != ERR_NONE){
        component_free(c);
        return ERR_MEM;
    }
    return ERR_NONE;
}