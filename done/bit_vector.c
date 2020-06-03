/**
 * @file bit_vector.c
 * @brief manipulation of big bit_vector for GameBoy Emulator
 *
 * @author C la vie
 * @date 2020
 */
#include "bit_vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

//nb of bit in one content in the struct bit_vector_t
#define VECTOR_SIZE 32
//nb of vector of 32 bit in a bit_vector_t
#define VECTORS_IN(pbv) (pbv->size % VECTOR_SIZE == 0 ? pbv->size / VECTOR_SIZE : pbv->size / VECTOR_SIZE + 1)
#define BV_1_VALUE (0xFFFFFFFF)
#define BV_0_VALUE (0x00000000)

typedef enum
{
    ZERO,
    WRAPPED
} extention_t;

bit_vector_t *bit_vector_create(size_t size, bit_t value)
{
    if (size == 0)
        return NULL;

    size_t nb = size % VECTOR_SIZE == 0 ? size / VECTOR_SIZE : size / VECTOR_SIZE + 1;
    bit_vector_t *pbv = NULL;
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
                    pbv->content[i] = value == 1u ? BV_1_VALUE : BV_0_VALUE;
                }
            }
            else
            {
                pbv->content[nb - 1] = value == 1u ? (BV_1_VALUE >> 32 - (size - (nb - 1) * VECTOR_SIZE)) : BV_0_VALUE;
                for (size_t i = 0; i < nb - 1; i++)
                {
                    pbv->content[i] = value == 1u ? BV_1_VALUE : BV_0_VALUE;
                }
            }
        }
    }
    return pbv;
}

bit_vector_t *bit_vector_cpy(const bit_vector_t *pbv)
{
    if (pbv == NULL)
        return NULL;

    bit_vector_t *copy = bit_vector_create(pbv->size, 0u);
    for (size_t i = 0; i < VECTORS_IN(pbv); i++)
    {
        copy->content[i] = pbv->content[i];
    }
    return copy;
}

bit_t bit_vector_get(const bit_vector_t *pbv, size_t index)
{
    if (pbv == NULL || index >= pbv->size)
        return 0;

    size_t index_of_the_vector = index / VECTOR_SIZE;
    size_t index_in_vector = index % VECTOR_SIZE;
    return (bit_t)((pbv->content[index_of_the_vector] & (1u << index_in_vector)) >> index_in_vector);
}

bit_vector_t *bit_vector_not(bit_vector_t *pbv)
{
    if (pbv == NULL)
        return NULL;

    for (size_t i = 0; i < VECTORS_IN(pbv); i++)
    {
        pbv->content[i] = ~pbv->content[i];
    }
    //set the last result that are not considered to be in the content to 0
    if (pbv->size % VECTOR_SIZE != 0)
    {
        size_t nb_element_to_reset = VECTOR_SIZE - (pbv->size % VECTOR_SIZE);
        pbv->content[pbv->size / VECTOR_SIZE - 1] = (pbv->content[pbv->size / VECTOR_SIZE - 1] << nb_element_to_reset) >> nb_element_to_reset;
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
        pbv1->content[i] &= pbv2->content[i];
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
        pbv1->content[i] |= pbv2->content[i];
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
        pbv1->content[i] ^= pbv2->content[i];
    }
    return pbv1;
}

//=========================================================================
/**
 * @brief Helper function that combine a part of the vector for wrap extraction
 * @param pbv, pointer to bit_vector
 * @param index_in_vector index from where to take the value in given part of the vector
 * @param index_of_the_vector index of the content to consider
 * @return value of the combined bit string (32 bits)
 */
uint32_t combine_wrap(const bit_vector_t *pbv, int64_t index_in_vector, int64_t index_of_the_vector)
{
    uint32_t result = 0;
    if (index_in_vector == 0)
    {
        result = pbv->content[index_of_the_vector % VECTORS_IN(pbv)];
    }
    else
    {
        //MSBs of indexed vector starting from index_in_vector are the result's LSBs
        result = pbv->content[index_of_the_vector % VECTORS_IN(pbv)] >> index_in_vector;
        //LSBs of next indexed vector are the result's LSBs
        result |= pbv->content[(index_of_the_vector + 1) % VECTORS_IN(pbv)] << (VECTOR_SIZE - index_in_vector);
    }

    return result;
}

//=========================================================================
/**
 * @brief Helper function that combine a part of the vector for zero extraction
 * @param pbv, pointer to bit_vector
 * @param index_in_vector index from where to take the value in given part of the vector
 * @param index_of_the_vector index of the content to consider
 * @return value of the combined bit string (32 bits)
 */
uint32_t combine_zero(const bit_vector_t *pbv, int64_t index_in_vector, int64_t index_of_the_vector)
{

    uint32_t result = 0;
    if (index_in_vector == 0)
    {
        if (index_of_the_vector >= 0 && index_of_the_vector < VECTORS_IN(pbv))
            result = pbv->content[index_of_the_vector];
    }
    else
    {
        //MSBs of indexed vector starting from index_in_vector are the result's LSBs
        if (index_of_the_vector >= 0 && index_of_the_vector < VECTORS_IN(pbv))
            result = pbv->content[index_of_the_vector] >> index_in_vector;
        //LSBs of next indexed vector are the result's LSBs
        if ((index_of_the_vector + 1) >= 0 && (index_of_the_vector + 1) < VECTORS_IN(pbv))
            result |= pbv->content[index_of_the_vector + 1] << (VECTOR_SIZE - index_in_vector);
    }
    return result;
}

//=========================================================================
/**
 * @brief Helper function that perform either wrap or zero infinite extraction
 * @param pbv, pointer to bit_vector
 * @param index index from where to start extraction
 * @param size size in bit of new bit vector
 * @param type, type of the extention
 * @return pointer new bit vector
 */
bit_vector_t *extract(const bit_vector_t *pbv, int64_t index, size_t size, extention_t type)
{
    bit_vector_t *result = bit_vector_create(size, 0u);
    int64_t index_of_the_vector = floor(index / (double)VECTOR_SIZE);
    int64_t index_in_vector = index % VECTOR_SIZE;
    if (index_in_vector < 0)
        index_in_vector += VECTOR_SIZE;

    for (size_t i = 0; i < VECTORS_IN(result); i++)
    {
        switch (type)
        {
        case WRAPPED:
            result->content[i] = combine_wrap(pbv, index_in_vector, index_of_the_vector++);
            break;
        case ZERO:
            result->content[i] = combine_zero(pbv, index_in_vector, index_of_the_vector++);
            break;
        }
    }
    return result;
}

bit_vector_t *bit_vector_extract_zero_ext(const bit_vector_t *pbv, int64_t index, size_t size)
{
    if (size == 0)
        return NULL;
    if (pbv == NULL)
        return bit_vector_create(size, 0u);
    else
        return extract(pbv, index, size, ZERO);
}

bit_vector_t *bit_vector_extract_wrap_ext(const bit_vector_t *pbv, int64_t index, size_t size)
{
    if (size == 0 || pbv == NULL)
        return NULL;

    bit_vector_t *result = extract(pbv, index, size, WRAPPED);

    if (pbv->size % VECTOR_SIZE == 0)
    {
        return result;
    }
    else
    {
        result = bit_vector_create(size, 0u);
        size_t start_index = index % pbv->size;
        for (size_t i = 0; i < size; i++)
        {
            uint32_t bit_to_set = bit_vector_get(pbv, (start_index + i) % pbv->size);
            bit_to_set <<= i % VECTOR_SIZE;
            result->content[i / VECTOR_SIZE] |= bit_to_set;
        }
        return result;
    }
}

bit_vector_t *bit_vector_shift(const bit_vector_t *pbv, int64_t shift)
{
    if (pbv == NULL)
        return NULL;
    else
        return bit_vector_extract_zero_ext(pbv, -shift, pbv->size);
}
bit_vector_t *bit_vector_join(const bit_vector_t *pbv1, const bit_vector_t *pbv2, int64_t shift)
{
    if (pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size || pbv1->size < shift || shift < 0)
        return NULL;
    bit_vector_t *result = bit_vector_create(pbv1->size, 0);
    for (size_t i = 0; i < shift / VECTOR_SIZE; i++)
    {
        result->content[i] = pbv1->content[i];
    }
    for (size_t i = shift / VECTOR_SIZE; i < VECTORS_IN(pbv1); i++)
    {
        result->content[i] = pbv2->content[i];
    }
    //handle the midle case
    if (shift % VECTOR_SIZE != 0)
    {
        uint32_t pbv1_part = (pbv1->content[shift / VECTOR_SIZE]
                              << (VECTOR_SIZE - shift % VECTOR_SIZE)) >>
                             (VECTOR_SIZE - shift % VECTOR_SIZE);
        uint32_t pbv2_part = (pbv2->content[shift / VECTOR_SIZE] >> shift % VECTOR_SIZE) << shift % VECTOR_SIZE;
        result->content[shift / VECTOR_SIZE] = pbv1_part | pbv2_part;
    }
    return result;
}

int bit_vector_print(const bit_vector_t *pbv)
{
    for (size_t i = pbv->size; i-- > 0;)
    {
        fprintf(stdout, "%" PRIu8, bit_vector_get(pbv, i));
    }
    return (int)pbv->size;
}

int bit_vector_println(const char *prefix, const bit_vector_t *pbv)
{
    int nb_of_char = 0;
    nb_of_char += fprintf(stdout, "%s", prefix);
    nb_of_char += bit_vector_print(pbv);
    nb_of_char += fprintf(stdout, "\n");
    return nb_of_char;
}

void bit_vector_free(bit_vector_t **pbv)
{
    free(*pbv);
    pbv = NULL;
}