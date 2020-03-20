#include <stdint.h>   // for uint8_t and uint16_t types
#include "alu.h"
#include "bit.h"
#include "error.h"
// ======================================================================
/**
* @brief get flag value
*
* @param flags flag set to get flag from
* @param flag flag value to get
* @return flag value
*/
flag_bit_t get_flag(flags_t flags, flag_bit_t flag) {
    switch(flag) {
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

/**
* @brief set flag
*
* @param flags (modified) set of flags
* @param flag flag to be set
*/
void set_flag(flags_t* flags, flag_bit_t flag) {
    switch(flag) {
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
        *flags = *flags |FLAG_C;
        break;
    }
}
/**
 * @brief set the flags of result alu_output structure with respect to
 * a couple of parameters
 *
 * @param result
 * @param half_carry
 * @param carry
 * @param indicate either its a subtraction or not
 */
void handle_flag_setting(alu_output_t* result, bit_t half_carry, bit_t carry, int subtraction){
    if(half_carry !=0) set_H(&result->flags);
    if(carry !=0) set_C(&result->flags);
    if(result->value == 0) set_Z(&result->flags);
    if(subtraction) set_N(&result->flags);
}

/**
 * @brief adds two uint8 and writes the results and flags into an alu_output_t structure
 *
 * @param result alu_output_t pointer to write into
 * @param x value to sum
 * @param y value to sum
 * @param c0 carry in
 * @return error code
 */
int alu_add8(alu_output_t* result, uint8_t x, uint8_t y, bit_t c0) {
	M_REQUIRE_NON_NULL(result);
    uint8_t sum_lsb = lsb4(x) + lsb4(y) + c0;
    uint8_t c4 = msb4(sum_lsb);
    uint8_t sum_msb = msb4(x) + msb4(y) + c4;
    uint8_t c8 = msb4(sum_msb);
    result->value = merge4(sum_lsb, sum_msb);
    handle_flag_setting(result, c4, c8, 0);
    return ERR_NONE;
}

/**
 * @brief subtract two uint8 and writes the results and flags into an alu_output_t structure
 *
 * @param result alu_output_t pointer to write into
 * @param x value to subtract from
 * @param y value to subtract
 * @param b0 initial borrow bit
 * @return error code
 */
int alu_sub8(alu_output_t* result, uint8_t x, uint8_t y, bit_t b0) {
	M_REQUIRE_NON_NULL(result);
    uint8_t sub_lsb = lsb4(x) - lsb4(y) - b0;
    uint8_t b4 = msb4(sub_lsb);
    uint8_t sub_msb = msb4(x) - msb4(y) + b4;
    uint8_t b8 = lsb4(sub_msb);
    result->value = merge4(sub_lsb, sub_msb);
    handle_flag_setting(result, b4, b8, TRUE);
    return ERR_NONE;
    
}

void alu_add16_option(alu_output_t* result, uint16_t x, uint16_t y, add_flag_option opt){
    alu_output_t result_low;
    alu_output_t result_high;
    alu_add8(&result_low, lsb8(x), lsb8(y), 0);
    bit_t c8 = get_C(result_low.flags) != 0 ? 1 : 0;
    alu_add8(&result_high, msb8(x), msb8(y),c8 );
    result->value = merge8(result_low.value,result_high.value);
    alu_output_t* choice = NULL;
    switch (opt){
        case LOW:
            choice = &result_low;
            break;
        case HIGH:
            choice = &result_high;
            break;
    }
    uint8_t half_carry = get_H(choice->flags);
    uint8_t carry = get_C(choice->flags);
    handle_flag_setting(result, half_carry, carry, FALSE);
}

/**
 * @brief sum two uint16 and writes the results and flags into an alu_output_t structure,
 *        the H & C flags are being placed according to the 8 lsb
 *
 * @param result alu_output_t pointer to write into
 * @param x value to sum
 * @param y value to sum
 * @return error code
 */
int alu_add16_low(alu_output_t* result, uint16_t x, uint16_t y) {
    M_REQUIRE_NON_NULL(result);
    alu_add16_option(result, x, y, LOW);
    return ERR_NONE;
}

/**
 * @brief sum two uint16 and writes the results and flags into an alu_output_t structure,
 *        the H & C flags are being placed according to the 8 msb
 *
 * @param result alu_output_t pointer to write into
 * @param x value to sum
 * @param y value to sum
 * @return error code
 */
int alu_add16_high(alu_output_t* result, uint16_t x, uint16_t y) {
    M_REQUIRE_NON_NULL(result);
    alu_add16_option(result, x, y, HIGH);
    return ERR_NONE;
}

/**
 * @brief logic shift
 *
 * @param result alu_output_t pointer to write into
 * @param x value to shift
 * @param dir shift direction
 * @return error code
 */
int alu_shift(alu_output_t* result, uint8_t x, rot_dir_t dir) {
	M_REQUIRE_NON_NULL(result);
	M_REQUIRE(dir == LEFT | dir==RIGHT, ERR_BAD_PARAMETER, "input value dir (%u) is not LEFT or RIGHT",dir);
    uint8_t carry = 0;
	if(dir == LEFT) {
        result->value = (uint8_t)(x << 1u);
        carry = bit_get(x, 7);
    } else if(dir == RIGHT) {
        result->value = (uint8_t)(x >> 1u);
        carry = bit_get(x, 0);
    }
    handle_flag_setting(result, 0, carry, FALSE);
    return ERR_NONE;
}

/**
 * @brief arithmetic shift
 *
 * @param result alu_output_t pointer to write into
 * @param x value to shift
 * @return error code
 */
int alu_shiftR_A(alu_output_t* result, uint8_t x) {
	M_REQUIRE_NON_NULL(result);
    bit_t ejected_bit = bit_get(x,0);
    uint8_t carry = ejected_bit!=0;
    uint8_t mask_sign_bit = x & 0x80u;
    result->value = x >> 1u;
    result->value |= mask_sign_bit;
    handle_flag_setting(result, 0, carry, FALSE);
    return ERR_NONE;
}

/**
 * @brief logic rotate
 *
 * @param result alu_output_t pointer to write into
 * @param x value to rotate
 * @param dir rotation direction
 * @return error code
 */
int alu_rotate(alu_output_t* result, uint8_t x, rot_dir_t dir) {
	M_REQUIRE_NON_NULL(result);
	M_REQUIRE(dir==LEFT | dir==RIGHT, ERR_BAD_PARAMETER, "input value dir (%u) is not LEFT or RIGHT",dir);
    bit_t msb = x & 0x80u;
    bit_t lsb = x & 0x01u;
    uint8_t carry = 0;
    if((dir == LEFT && msb)||(dir == RIGHT && lsb)) carry = 1;
    bit_rotate(&x, dir, 1);
    result->value = x;
    handle_flag_setting(result, 0, carry, FALSE);
    return ERR_NONE;
}


/**
 * @brief logic rotate with carry taken into account
 *
 * @param result alu_output_t pointer to write into
 * @param x value to rotate
 * @param dir rotation direction
 * @param flags carry flag
 * @return error code
 */
int alu_carry_rotate(alu_output_t* result, uint8_t x, rot_dir_t dir, flags_t flags) {
	M_REQUIRE_NON_NULL(result);
	M_REQUIRE(dir == LEFT | dir==RIGHT, ERR_BAD_PARAMETER, "input value dir (%u) is not LEFT or RIGHT",dir);
    bit_t newCarry = (dir == RIGHT) ? (x & 0x01u) : (x & 0x80u);
    x = (dir == RIGHT) ? (x>>1u) : (x<<1u);
    if(get_C(flags)!=0) x = (dir == RIGHT) ? (x | 0x80u) : (x | 0x01u);
    result->value = x;
    handle_flag_setting(result, 0, newCarry, FALSE);
    return ERR_NONE;
}
