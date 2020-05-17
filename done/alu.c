/**
 * @file alu.c
 * @brief ALU for GameBoy Emulator
 *
 * @author C la vie
 * @date 2020
 */

#include "alu.h"
#include "error.h"
#include <stdio.h>

/**
 * @brief type add_flag_option allows to choose the functions between alu_add16_low and alu_add16_high
 */
typedef enum
{
    LOW,
    HIGH
} add_flag_option;

/**
 * @brief type define the boolean values in a simple way
 */
typedef enum
{
    TRUE = 1,
    FALSE = 0
} boolean;

flag_bit_t get_flag(flags_t flags, flag_bit_t flag)
{
    switch (flag)
    {
    case FLAG_Z:
        return FLAG_Z & flags;
    case FLAG_N:
        return FLAG_N & flags;
    case FLAG_H:
        return FLAG_H & flags;
    case FLAG_C:
        return FLAG_C & flags;
    //the flag doesn't match any cases
    default:
        return 0;
    }
}

void set_flag(flags_t *flags, flag_bit_t flag)
{
    if (flags != NULL)
    {
        switch (flag)
        {
        case FLAG_Z:
            *flags = *flags | FLAG_Z;
            break;
        case FLAG_N:
            *flags = *flags | FLAG_N;
            break;
        case FLAG_H:
            *flags = *flags | FLAG_H;
            break;
        case FLAG_C:
            *flags = *flags | FLAG_C;
            break;
        default:
            fprintf(stderr, "Error - Bad parameter not a flag_bit_t %d", flag);
            break;
        }
    }
    else
    {
        fprintf(stderr, "Error - NULLpointer the given flags_t pointer is NULL");
    }
}
/**
 * @brief set the flags of result with respect to a couple of parameters
 *
 * @param result alu_output_t pointer to write into
 * @param half_carry half carry but
 * @param carry carry bit
 * @param substraction indicates whether it is a subtraction or not
 */
void handle_flag_setting(alu_output_t *result, bit_t half_carry, bit_t carry, boolean subtraction)
{
    if (half_carry != 0)
        set_H(&result->flags);
    if (carry != 0)
        set_C(&result->flags);
    if (result->value == 0)
        set_Z(&result->flags);
    if (subtraction)
        set_N(&result->flags);
}

int alu_add8(alu_output_t *result, uint8_t x, uint8_t y, bit_t c0)
{
    M_REQUIRE_NON_NULL(result);

    uint8_t sum_lsb = (uint8_t)(lsb4(x) + lsb4(y) + c0);
    uint8_t c4 = msb4(sum_lsb);

    uint8_t sum_msb = (uint8_t)(msb4(x) + msb4(y) + c4);
    uint8_t c8 = msb4(sum_msb);

    result->value = merge4(sum_lsb, sum_msb);
    handle_flag_setting(result, c4, c8, FALSE);

    return ERR_NONE;
}

int alu_sub8(alu_output_t *result, uint8_t x, uint8_t y, bit_t b0)
{
    M_REQUIRE_NON_NULL(result);

    uint8_t sub_lsb = (uint8_t)(lsb4(x) - lsb4(y) - b0);
    uint8_t b4 = msb4(sub_lsb);

    bit_t half_carry = bit_get(sub_lsb, 4);
    uint8_t sub_msb = (uint8_t)(msb4(x) - msb4(y) + b4);
    bit_t carry = bit_get((msb4(x) - msb4(y) - half_carry), 4);
    uint8_t b8 = msb4(sub_msb);

    result->value = merge4(sub_lsb, sub_msb);
    handle_flag_setting(result, half_carry, carry, TRUE);

    return ERR_NONE;
}

/**
 * @brief computes alu_add16_low or alu_add_16_high depending on the option opt
 *
 * @param result alu_output_t pointer to write into
 * @param x value to sum
 * @param y value to sum
 * @param opt option which lets us choose between low and high
 */
void alu_add16_option(alu_output_t *result, uint16_t x, uint16_t y, add_flag_option opt)
{
    alu_output_t result_low = {0};
    alu_output_t result_high = {0};

    alu_add8(&result_low, lsb8(x), lsb8(y), 0);
    bit_t c8 = get_C(result_low.flags) != 0 ? 1 : 0;
    alu_add8(&result_high, msb8(x), msb8(y), c8);

    result->value = merge8((uint8_t)result_low.value, (uint8_t)result_high.value);
    alu_output_t *choice = NULL;

    switch (opt)
    {
    case LOW:
        choice = &result_low;
        break;
    case HIGH:
        choice = &result_high;
        break;
    default:
        fprintf(stderr, "Error - Bad parameter not an add_flag_option%d", opt);
        break;
    }

    uint8_t half_carry = get_H(choice->flags);
    uint8_t carry = get_C(choice->flags);
    handle_flag_setting(result, half_carry, carry, FALSE);
}

int alu_add16_low(alu_output_t *result, uint16_t x, uint16_t y)
{
    M_REQUIRE_NON_NULL(result);
    alu_add16_option(result, x, y, LOW);
    return ERR_NONE;
}

int alu_add16_high(alu_output_t *result, uint16_t x, uint16_t y)
{
    M_REQUIRE_NON_NULL(result);
    alu_add16_option(result, x, y, HIGH);
    return ERR_NONE;
}

int alu_shift(alu_output_t *result, uint8_t x, rot_dir_t dir)
{
    M_REQUIRE_NON_NULL(result);
    M_REQUIRE((dir == LEFT) | (dir == RIGHT), ERR_BAD_PARAMETER, "input value dir (%u) is not LEFT or RIGHT", dir);

    uint8_t carry = 0;
    if (dir == LEFT)
    {
        result->value = (uint8_t)(x << 1u);
        carry = bit_get(x, 7);
    }
    else if (dir == RIGHT)
    {
        result->value = (uint8_t)(x >> 1u);
        carry = bit_get(x, 0);
    }

    handle_flag_setting(result, 0, carry, FALSE);

    return ERR_NONE;
}

int alu_shiftR_A(alu_output_t *result, uint8_t x)
{
    M_REQUIRE_NON_NULL(result);

    bit_t ejected_bit = bit_get(x, 0);
    uint8_t carry = ejected_bit != 0;
    uint16_t mask_sign_bit = x & 0x80u;

    result->value = x >> 1u;
    result->value |= mask_sign_bit;

    handle_flag_setting(result, 0, carry, FALSE);

    return ERR_NONE;
}

int alu_rotate(alu_output_t *result, uint8_t x, rot_dir_t dir)
{
    M_REQUIRE_NON_NULL(result);
    M_REQUIRE((dir == LEFT) | (dir == RIGHT), ERR_BAD_PARAMETER, "input value dir (%u) is not LEFT or RIGHT", dir);

    bit_t msb = x & 0x80u;
    bit_t lsb = x & 0x01u;
    uint8_t carry = 0;

    if ((dir == LEFT && msb) || (dir == RIGHT && lsb))
        carry = 1;
    bit_rotate(&x, dir, 1);

    result->value = x;
    handle_flag_setting(result, 0, carry, FALSE);

    return ERR_NONE;
}

int alu_carry_rotate(alu_output_t *result, uint8_t x, rot_dir_t dir, flags_t flags)
{
    M_REQUIRE_NON_NULL(result);
    M_REQUIRE((dir == LEFT) | (dir == RIGHT), ERR_BAD_PARAMETER, "input value dir (%u) is not LEFT or RIGHT", dir);

    bit_t newCarry = (dir == RIGHT) ? (x & 0x01u) : (x & 0x80u);
    x = (dir == RIGHT) ? (uint8_t)(x >> 1u) : (uint8_t)(x << 1u);

    if (get_C(flags) != 0)
        x = (dir == RIGHT) ? (x | 0x80u) : (x | 0x01u);

    result->value = x;
    handle_flag_setting(result, 0, newCarry, FALSE);

    return ERR_NONE;
}
