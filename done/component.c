/**
 * @file component.c
 * @brief Game Boy Component simulation 
 *
 * @author C la vie
 * @date 2020
 */
 
#include "component.h"
#include "error.h"
#include "memory.h"

int component_create(component_t *c, size_t mem_size) {
    M_REQUIRE_NON_NULL(c);

    c->start = 0;
    c->end = 0;
    if(mem_size == 0) {
        c->mem = NULL;
        return ERR_NONE;
    }
    c->mem = malloc(sizeof(memory_t));

    //check the c-mem is done
    //et vérifier mem_create
    mem_create(c->mem, mem_size);
    
    return ERR_NONE;
}

void component_free(component_t *c) {
    if (c != NULL) {
        mem_free(c->mem);
        //TODO free after the malloc
        c->mem = NULL;
        c->start = 0;
        c->end = 0;
    }
}

int component_shared(component_t* c, component_t* c_old) {
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c_old);
    M_REQUIRE_NON_NULL_CUSTOM_ERR(c_old->mem, ERR_MEM);

    c->start = 0;
    c->end = 0;
    c->mem = c_old->mem;
    
    return ERR_NONE;
}
