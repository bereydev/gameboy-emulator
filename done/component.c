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
    return mem_create(c->mem, mem_size); 
}

void component_free(component_t *c) {
    if (c != NULL) {
        mem_free(c->mem);
        free(c->mem);
        c->mem = NULL;
        c->start = 0;
        c->end = 0;
    }
}

int component_shared(component_t* c, component_t* c_old) {
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c_old);

    c->start = 0;
    c->end = 0;
    c->mem = c_old->mem;
    
    return ERR_NONE;
}
