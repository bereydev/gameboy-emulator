#include <stdlib.h>
#include "memory.h"
#include "error.h"
#include <stdint.h> // pour SIZE_MAX


int mem_create(memory_t* mem, size_t size){
    M_REQUIRE_NON_NULL(mem);
    //il faut empecher la création d'une mémoire de taille null. Borne supérieure nécessaire?
    M_REQUIRE(0 < size, ERR_BAD_PARAMETER, "\"input size (%lu) is incorrect \"", size);
    //TO DO Est-ce que c'est mieux d'éviter de créer result mais de travailler sur le pointeur?
   
    /*memory_t result = {0, NULL};
    result.memory = calloc(size, sizeof(data_t));
    if (result.memory != NULL)
        result.size = size;
    else
		return ERR_MEM;
        
    *mem = result;
    return ERR_NONE;*/
    mem->size = 0;
    mem->memory = calloc(size, sizeof(data_t));
    M_EXIT_IF_NULL(mem->memory, size);
    mem->size = size;

    return ERR_NONE;
}

void mem_free(memory_t* mem){
    if (mem != NULL && mem->memory!=NULL) {
        free(mem->memory);
        mem->memory = NULL;
        mem->size = 0;
    }
}
