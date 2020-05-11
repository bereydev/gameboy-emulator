/**
 * @file memory.c
 * @brief Memory for Game
 *
 * @date 2020
 */
 
#include <stdlib.h>
#include "memory.h"
#include "error.h"

int mem_create(memory_t* mem, size_t size) {
    M_REQUIRE_NON_NULL(mem);
    M_REQUIRE(0 < size, ERR_BAD_PARAMETER, "\"input size (%lu) is incorrect \"", size);
    mem->size = 0;
    mem->memory = calloc(size, sizeof(data_t));
    M_EXIT_IF_NULL(mem->memory, size);
    mem->size = size;

    return ERR_NONE;
}

void mem_free(memory_t* mem) {
    if (mem != NULL && mem->memory!=NULL) {
        free(mem->memory);
        mem->memory = NULL;
        mem->size = 0;
    }
}
