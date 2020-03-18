#include <stdint.h>   // for uint8_t and uint16_t types
#include "alu.h"
#include "bit.h"
// ======================================================================
/**
* @brief get flag value
*
* @param flags flag set to get flag from
* @param flag flag value to get
* @return flag value
*/
flag_bit_t get_flag(flags_t flags, flag_bit_t flag){
	switch(flag) {
		case FLAG_Z: FLAG_Z & flags; break;
		case FLAG_N: FLAG_N & flags; break;
		case FLAG_H: FLAG_H & flags; break;
		case FLAG_C: FLAG_C & flags; break;
		//the flag doesn't match any cases
		default: return 0;
		}
}

/**
* @brief set flag
*
* @param flags (modified) set of flags
* @param flag flag to be set
*/
void set_flag(flags_t* flags, flag_bit_t flag){
	switch(flag) {
		case FLAG_Z: FLAG_Z | flags; break;
		case FLAG_N: FLAG_N | flags; break;
		case FLAG_H: FLAG_H | flags; break;
		case FLAG_C: FLAG_C | flags; break;
	}
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
int alu_add8(alu_output_t* result, uint8_t x, uint8_t y, bit_t c0){
	uint8_t sum_lsb = lsb4(x) + lsb4(y) + c0;
	uint8_t c4 = msb4(sum_lsb);
	if(c4!=0) set_flag(*result.fanions, FLAG_H);
	uint8_t sum_msb = msb4(x) + msb4(y) + c4;
	uint8_t c8 = msb4(sum_msb);
	if(c8!=0) set_flag(*result.fanions, FLAG_C);
	*result.value = merge4(sum_lsb, sum_msb);
	if(*result.value==0)  set_flag(*result.fanions, FLAG_Z);
	return *result.value;
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
int alu_sub8(alu_output_t* result, uint8_t x, uint8_t y, bit_t b0){
	set_flag(*result.fanions, FLAG_N);
	uint8_t sub_lsb= lsb4(x) - lsb4(y) - b0;
	uint8_t b4 = msb4(sub_lsb);
	if(b4!=0) set_flag(*result.fanions, FLAG_H);
	uint8_t sub_msb = msb4(x) - msb4(y) + b4;
	uint8_t b8 = msb4(sub_msb);
	if(b8!=0) set_flag(*result.fanions, FLAG_C);
	*result.value = merge4(sub_lsb, sub_msb);
	if(*result.value==0)  set_flag(*result.fanions, FLAG_Z);
	return *result.value;
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
int alu_add16_low(alu_output_t* result, uint16_t x, uint16_t y){
	alu_output_t result_high;
	uint16_t sum8lsb = alu_add8(*result, lsb8(x), lsb8(y), 0);
	uint16_t sum8msb = alu_add8(result_high, msb(x), msb(y), msb8(sum8lsb));
	*result.value= merge8(sum8lsb,sum8msb);
	if(*result.value==0)  set_flag(*result.fanions, FLAG_Z);
	return *result.value;
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
int alu_add16_high(alu_output_t* result, uint16_t x, uint16_t y){
	alu_output_t result_low;
	uint16_t sum8lsb = alu_add8(result_low, lsb8(x), lsb8(y), 0);
	uint16_t sum8msb = alu_add8(*result, msb(x), msb(y), msb8(sum8lsb));
	*result.value= merge8(sum8lsb,sum8msb);
	if(*result.value==0)  set_flag(*result.fanions, FLAG_Z);
	return *result.value;
}

/**
 * @brief logic shift
 *
 * @param result alu_output_t pointer to write into
 * @param x value to shift
 * @param dir shift direction
 * @return error code
 */
int alu_shift(alu_output_t* result, uint8_t x, rot_dir_t dir){
	if(dir == LEFT) {
		*result.value = x << 1;
		if(bit_get(x, 7) !=0) set_flag(*result.fanions, FLAG_C);
	}else if(dir == RIGHT) {
		*result.value = x >> 1;
		if(bit_get(x, 0) !=0) set_flag(*result.fanions, FLAG_C);
		}
	if (*result.value ==0) set_flag(*result.fanions, FLAG_Z);
}

/**
 * @brief arithmetic shift
 *
 * @param result alu_output_t pointer to write into
 * @param x value to shift
 * @return error code
 */
int alu_shiftR_A(alu_output_t* result, uint8_t x){
	bit_t ejected_bit = bit_get(x,7);
	if(ejected_bit!=0) set_flag(*result.fanions, FLAG_C);
	uint8_t mask_sign_bit = x & 0x80;
	*result.value = x >> 1;
	*result.value |= mask_sign_bit;
	if (*result.value ==0) set_flag(*result.fanions, FLAG_Z);
	}
	
/**
 * @brief logic rotate
 *
 * @param result alu_output_t pointer to write into
 * @param x value to rotate
 * @param dir rotation direction
 * @return error code
 */
int alu_rotate(alu_output_t* result, uint8_t x, rot_dir_t dir){
	bit_t msb = x & 0x80;
	bit_t lsb = x & 0x01;
	if(dir==LEFT && msb!=0) set_flag(*result.fanions, FLAG_C);
	else if(dir == RIGHT && lsb!=0) set_flag(*result.fanions, FLAG_C);
	bit_rotate(*x, dir, 1);
	*result.value = x;
	if(*result.value == 0) set_flag(*result.fanions, FLAG_Z);
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
int alu_carry_rotate(alu_output_t* result, uint8_t x, rot_dir_t dir, flags_t flags){
	bit_t newCarry = (dir == RIGHT) ? (x & 0x01) : (x & 0x80);
	if(newCarry !=0) set_flag(*result.fanions, FLAG_C);
	x = (dir == RIGHT) ? (x>>1) : (x<<1);
	if(newCarry!=0) x = (dir == RIGHT) ? (x | 0x80) : (x | 0x10);
	*result.value = x;
	if(*result.value == 0) set_flag(*result.fanions, FLAG_Z);
	}




