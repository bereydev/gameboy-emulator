#include "component.h"
#include "error.h"
#include "memory.h"

int component_create(component_t *c, size_t mem_size) {
    M_REQUIRE_NON_NULL(c);
    /* initialise les éléments du composant (end et start à 0
     * sert de convention pour indiquer que le composant n'est pas
     * connecté au bus
    */
    c->start = 0;
    c->end = 0;
    if(mem_size == 0){
		//le composant n'a pas de mémoire et la fonction retourne ERR_NONE
		//un pointeur NULL indique que le composant n'a pas de mémoire
		c->mem = NULL;
		return ERR_NONE;
		}
    c->mem = malloc(sizeof(memory_t));
    mem_create(c->mem, mem_size);
    return ERR_NONE;
}

// ======================================================================

void component_free(component_t *c) {
    if (c != NULL) {
        mem_free(c->mem);
        c->mem = NULL;
        c->start = 0;
        c->end = 0;
    }
}

// ======================================================================

int component_shared(component_t* c, component_t* c_old){
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c_old);

    c->start = 0;
    c->end = 0;
    c->mem = c_old->mem;
    return ERR_NONE;
}


