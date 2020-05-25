/**
 * @file bit_vector.c
 * @brief manipulation of big bit_vector for GameBoy Emulator
 *
 * @author C la vie
 * @date 2020
 */

#include "bit_vector.h"
#include <stdio.h>
#include <inttypes.h>

//nb of bit in one content in the struct bit_vector_t
#define VECTOR_SIZE 32

bit_vector_t *bit_vector_create(size_t size, bit_t value)
{
    if (size <= 0)
    {
        return NULL;
    }

    bit_vector_t *pbv = NULL;
    size_t nb = size % VECTOR_SIZE == 0 ? size / VECTOR_SIZE : size / VECTOR_SIZE + 1;
    pbv = malloc(sizeof(bit_vector_t) + (nb - 1) * sizeof(uint32_t));
    const size_t N_MAX = (SIZE_MAX - sizeof(bit_vector_t)) / sizeof(uint32_t) + 1;
    if (nb <= N_MAX)
    {
        pbv = malloc(sizeof(bit_vector_t) + (nb - 1) * sizeof(uint32_t));
        if (pbv != NULL)
        {
            pbv->size = size;
            if (size % VECTOR_SIZE == 0)
            {
                for (size_t i = 0; i < nb; i++)
                {
                    pbv->content[i] = value == 1u ? 0xFFFFFFFF : 0x00000000;
                }
            } else
            {
                pbv->content[nb - 1 ] = value == 1u ? 0xFFFFFFFF >> 32 - (size - (nb-1)*VECTOR_SIZE) : 0x00000000;
                for (size_t i = 0; i < nb-1; i++)
                {
                     pbv->content[i] = value == 1u ? 0xFFFFFFFF : 0x00000000;
                }
            }
            
        }
    }
    return pbv;
}

bit_vector_t *bit_vector_cpy(const bit_vector_t *pbv)
{
    if (pbv == NULL)
    {
        return NULL;
    }
    bit_vector_t *copy = bit_vector_create(pbv->size, 0u);
    for (size_t i = 0; i < pbv->size / VECTOR_SIZE; i++)
    {
        copy->content[i] = pbv->content[i];
    }
    return copy;
}

bit_t bit_vector_get(const bit_vector_t *pbv, size_t index)
{
    if (pbv == NULL || index >= pbv->size || index < 0u)
    {
        return 0;
    }
    
    size_t index_of_the_vector = index / VECTOR_SIZE ;
    size_t index_in_vector = (index % VECTOR_SIZE);

    return (bit_t)((pbv->content[index_of_the_vector] & ( 1u << index_in_vector)) >> index_in_vector );
}

bit_vector_t *bit_vector_not(bit_vector_t *pbv)
{
    if (pbv == NULL)
    {
        return NULL;
    }
    for (size_t i = 0; i < pbv->size / VECTOR_SIZE; i++)
    {
        fprintf(stderr, "valeur avant négation %"PRIu32"\n", pbv->content[i]);
        pbv->content[i] = ~pbv->content[i];
        fprintf(stderr, "valeur après négation %"PRIu32"\n", pbv->content[i]);
    }
    //set the last bits that are not considered to be in the content to 0
    if (pbv->size % VECTOR_SIZE != 0) {
        size_t nb_element_to_reset = VECTOR_SIZE - (pbv->size % VECTOR_SIZE);
        fprintf(stderr, "Nombre de shift à faire %zu \n", nb_element_to_reset);
        pbv->content[pbv->size/VECTOR_SIZE - 1] = (pbv->content[pbv->size/VECTOR_SIZE - 1] << nb_element_to_reset) >> nb_element_to_reset;
    }
    

    return pbv;
}

//TODO est-ce que pour les fonction and or etc il faut considérer le cas ou les vecteurs n'on pas la même taille ?
bit_vector_t *bit_vector_and(bit_vector_t *pbv1, const bit_vector_t *pbv2)
{
    if (pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size)
    {
        return NULL;
    }

    for (size_t i = 0; i < pbv1->size / VECTOR_SIZE; i++)
    {
        pbv1->content[i] = pbv1->content[i] & pbv2->content[i];
    }

    return pbv1;
}

bit_vector_t *bit_vector_or(bit_vector_t *pbv1, const bit_vector_t *pbv2)
{
    if (pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size)
    {
        return NULL;
    }

    for (size_t i = 0; i < pbv1->size / VECTOR_SIZE; i++)
    {
        pbv1->content[i] = pbv1->content[i] | pbv2->content[i];
    }

    return pbv1;
}

bit_vector_t *bit_vector_xor(bit_vector_t *pbv1, const bit_vector_t *pbv2)
{
    if (pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size)
    {
        return NULL;
    }

    for (size_t i = 0; i < pbv1->size / VECTOR_SIZE; i++)
    {
        pbv1->content[i] = pbv1->content[i] ^ pbv2->content[i];
    }

    return pbv1;
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
    free(*pbv);
    pbv = NULL;
}