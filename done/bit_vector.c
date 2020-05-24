/**
 * @file bit_vector.c
 * @brief manipulation of big bit_vector for GameBoy Emulator
 *
 * @author C la vie
 * @date 2020
 */

#include "bit_vector.h"

bit_vector_t *bit_vector_create(size_t size, bit_t value)
{
    // TODO les bits internes supplémentaires (implémentation) ayant une valeur non spécifiée (ceci dit, 0 facilite la tache)) ? 
    // Donc on alloue tout à 0 sauf les size premier bit ?
    size_t nb_of_full_vector = size/32;
    size_t nb_of_excess_bit = size - nb_of_full_vector * 32; 
    size_t nb = size % 32 == 0 ? size/32 : size/32 + 1; 
    const size_t N_MAX = (SIZE_MAX - sizeof(bit_vector_t)) / sizeof(uint32_t) + 1;
    if (nb <= N_MAX)
    {
        bit_vector_t *tab = malloc(sizeof(bit_vector_t) + (nb - 1) * sizeof(uint32_t));
        if (tab != NULL)
        {
            tab->size = nb;
            for (size_t i = 0; i < nb_of_full_vector; i++)
            {
                tab->vector[i] = value == 1u ? -1 : 0u; //-1 is equivalent to only 1 in usigned
            }
            tab->vector[nb-1] = 0u;
            for (size_t i = 0; i < nb_of_excess_bit; i++)
            {
                tab->vector[nb-1] += 1u;
                tab->vector[nb-1] << 1;
            }   
        }
    }
}

bit_vector_t *bit_vector_cpy(const bit_vector_t *pbv)
{
}

bit_t bit_vector_get(const bit_vector_t *pbv, size_t index)
{
}

bit_vector_t *bit_vector_not(bit_vector_t *pbv)
{
}

bit_vector_t *bit_vector_and(bit_vector_t *pbv1, const bit_vector_t *pbv2)
{
}

bit_vector_t *bit_vector_or(bit_vector_t *pbv1, const bit_vector_t *pbv2)
{
}

bit_vector_t *bit_vector_xor(bit_vector_t *pbv1, const bit_vector_t *pbv2)
{
}

bit_vector_t *bit_vector_extract_zero_ext(const bit_vector_t *pbv, int64_t index, size_t size)
{
}

bit_vector_t *bit_vector_extract_wrap_ext(const bit_vector_t *pbv, int64_t index, size_t size)
{
}

bit_vector_t *bit_vector_shift(const bit_vector_t *pbv, int64_t shift)
{
}

bit_vector_t *bit_vector_join(const bit_vector_t *pbv1, const bit_vector_t *pbv2, int64_t shift)
{
}

int bit_vector_print(const bit_vector_t *pbv)
{
}

int bit_vector_println(const char *prefix, const bit_vector_t *pbv)
{
}

void bit_vector_free(bit_vector_t **pbv)
{
}