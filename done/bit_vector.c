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
//nb of vector of 32 bit in a bit_vector_t
#define VECTORS_IN(pbv) (pbv->size % VECTOR_SIZE == 0 ? pbv->size / VECTOR_SIZE : pbv->size / VECTOR_SIZE + 1)

bit_vector_t *bit_vector_create(size_t size, bit_t value)
{
    if (size <= 0)
    {
        return NULL;
    }

    size_t nb = size % VECTOR_SIZE == 0 ? size / VECTOR_SIZE : size / VECTOR_SIZE + 1;
    bit_vector_t *pbv  = malloc(sizeof(bit_vector_t) + (nb - 1) * sizeof(uint32_t));
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
    //size_t nb = pbv->size % VECTOR_SIZE == 0 ? pbv->size / VECTOR_SIZE : pbv->size / VECTOR_SIZE + 1;
    for (size_t i = 0; i < VECTORS_IN(pbv); i++)
    {
        pbv->content[i] = ~pbv->content[i];
    }
    //set the last bits that are not considered to be in the content to 0
    if (pbv->size % VECTOR_SIZE != 0) {
        size_t nb_element_to_reset = VECTOR_SIZE - (pbv->size % VECTOR_SIZE);
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

    for (size_t i = 0; i < VECTORS_IN(pbv1); i++)
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

    for (size_t i = 0; i < VECTORS_IN(pbv1); i++)
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

    for (size_t i = 0; i < VECTORS_IN(pbv1); i++)
    {
        pbv1->content[i] = pbv1->content[i] ^ pbv2->content[i];
    }

    return pbv1;
}

bit_vector_t *bit_vector_extract_zero_ext(const bit_vector_t *pbv, int64_t index, size_t size)
{
    if (size <= 0) { return NULL;}
    bit_vector_t * result = bit_vector_create(size, 0u);
    if (pbv == NULL || index + size <= 0 || index >= 0 && (size_t)index >= pbv->size) {
        return result;
    }else
    {
        // copy is done from start(included) to end(excluded)
        int64_t start_copy_index = index < 0 ? 0 : index;
        int64_t end_copy_index = index + size <= pbv->size ? index + size : pbv->size ;
        int64_t size_of_copy = (end_copy_index - start_copy_index);
        int64_t start_replacement_index = index > 0 ? 0 : (0-index);
        
        for (size_t i = 0; i < size_of_copy; i++)
        {
            uint32_t bit_to_set = bit_vector_get(pbv, start_copy_index + i);
            bit_to_set = bit_to_set << (start_replacement_index + i) % VECTOR_SIZE;
            result->content[(start_replacement_index + i)/VECTOR_SIZE] = result->content[(start_replacement_index + i)/VECTOR_SIZE] | bit_to_set;
        }
    }

    return result;
    
}

bit_vector_t *bit_vector_extract_wrap_ext(const bit_vector_t *pbv, int64_t index, size_t size)
{
    if (size <= 0 || pbv == NULL) {return NULL;}
    //crer un vecteur remplit de 0
    bit_vector_t *result = bit_vector_create(size, 0u); 
    //calculer l'index de départ de la copie
    size_t start_index = index % pbv->size;
    //puis itterer de 0 à size en utilisant des modulos de pbv->size pour acceder au bon bit
    for (size_t i = 0; i < size; i++)
    {
        uint32_t bit_to_set = bit_vector_get(pbv, (start_index + i) % pbv->size);
        bit_to_set = bit_to_set << i % VECTOR_SIZE;
        result->content[i/VECTOR_SIZE] = result->content[i/VECTOR_SIZE] | bit_to_set;
    }

    return result;
    
}

bit_vector_t *bit_vector_shift(const bit_vector_t *pbv, int64_t shift)
{
    if (pbv == NULL){return NULL;}
    bit_vector_println("Avant shift :", pbv);
    bit_vector_t * result = bit_vector_extract_zero_ext(pbv, shift, pbv->size);
    fprintf(stderr, "Shift de : %"PRId64"\n", shift);
    bit_vector_println("Après shift :", result);

    return bit_vector_extract_zero_ext(pbv, shift, pbv->size);
}

bit_vector_t *bit_vector_join(const bit_vector_t *pbv1, const bit_vector_t *pbv2, int64_t shift)
{
    if (pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size || pbv1->size < shift || shift < 0 ) {return NULL;}
    size_t middle_case_index = shift%VECTOR_SIZE == 0 && shift != 0? shift/VECTOR_SIZE - 1 : shift/VECTOR_SIZE;
    bit_vector_t* result = bit_vector_create(pbv1->size, 0);
    for (size_t i = 0; i < shift/VECTOR_SIZE; i++)
    {
        result->content[i] = pbv1->content[i];
    }
    for (size_t i =  shift/VECTOR_SIZE; i < VECTORS_IN(pbv1); i++)
    {
        result->content[i] = pbv2->content[i];
    }
    //handle the midle case
    uint32_t pbv1_part = (pbv1->content[shift/VECTOR_SIZE] << (VECTOR_SIZE - shift % VECTOR_SIZE)) >> (VECTOR_SIZE - shift % VECTOR_SIZE);
    uint32_t pbv2_part = (pbv2->content[shift/VECTOR_SIZE] >> shift % VECTOR_SIZE) << shift % VECTOR_SIZE;
    result->content[shift/VECTOR_SIZE] = pbv1_part | pbv2_part;
    return result;
}

int bit_vector_print(const bit_vector_t *pbv)
{
    for (size_t i = pbv->size; i-- > 0;)
    {
        fprintf(stdout, "%"PRIu8, bit_vector_get(pbv,i));
    }
    
}

int bit_vector_println(const char *prefix, const bit_vector_t *pbv)
{
    fprintf(stdout, "%s", prefix);
    bit_vector_print(pbv);
    fprintf(stdout, "\n");
}

void bit_vector_free(bit_vector_t **pbv)
{
    // la je dois louper quelque chose
    free(*pbv);
    pbv = NULL;
}