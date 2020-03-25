#include "component.h"
#include "error.h"

/**
 * @brief Creates a component given various arguments
 *
 * @param c component pointer to initialize
 * @param mem_size size of the memory of the component
 * @return error code
 */
int component_create(component_t *c, size_t mem_size) {
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE(0 <= mem_size, ERR_BAD_PARAMETER, "input offset (%u) is incorrect", mem_size);
    /* initialise les éléments du composant (end et start à 0
     * sert de convention pour indiquer que le composant n'est pas
     * connecté au bus
    */
    c->start = 0;
    c->end = 0;
    if (mem_create(c->mem, mem_size) != ERR_NONE)
        mem_free(c->mem);
    return ERR_NONE;
}

/**
 * @brief Destroy's a component
 *
 * @param c component pointer to destroy
 */
void component_free(component_t *c) {
    if (c != NULL) {
        mem_free(c->mem);
        c->start = 0;
        c->end = 0;
    }
}
