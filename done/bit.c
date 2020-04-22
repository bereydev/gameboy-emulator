/**
 * @file bit.c
 * @brief Bit operations for GameBoy Emulator
 *
 * @author C la vie
 * @date 2020
 */

#include <stdint.h>   // for uint8_t and uint16_t types
#include "bit.h"

/**
 * @brief a type to represent 1 single bit.
 */
typedef uint8_t bit_t;

#define UINT8_T_SIZE 8

const uint8_t mask_lsb4 = 0x0f;
const uint8_t mask_msb4 = 0xf0;

uint8_t lsb4(uint8_t value){
    return value & mask_lsb4;
}

uint8_t msb4(uint8_t value){
    return (value & mask_msb4) >> 4;
}

bit_t bit_get(uint8_t value, int index){
    index = CLAMP07(index);
    return (bit_t)((value & (0x01 << index)) >> index);
}

void bit_set(uint8_t* value, int index){
    index = CLAMP07(index);
    *value = (uint8_t)((*value) | (0x01 << index));
}

void bit_unset(uint8_t* value, int index){
    index = CLAMP07(index);
    *value = (uint8_t)((*value) & ~(0x01 << index));
}

const uint16_t mask_lsb8 = 0x00ff;
const uint16_t mask_msb8 = 0xff00;

uint8_t lsb8(uint16_t value){
    return (uint8_t)(value & mask_lsb8);
}

uint8_t msb8(uint16_t value){
    return (uint8_t)((value & mask_msb8) >> 8);
}

uint8_t merge4(uint8_t value, uint8_t value2){
    return (uint8_t)(lsb4(value) | (value2<<4));
}

uint16_t merge8(uint8_t value, uint8_t value2){
    return (uint8_t)(value | (value2 << 8));
}

/**
 * @brief Rotates the bits in a uint8_t to the left
 *
 * @param num pointer to the number to rotate
 * @param rotation number of rotation steps to execute
 */
void rotateLeft(uint8_t* num, int rotation){
    *num = ((*num) >> (UINT8_T_SIZE-rotation)) | ((*num) << rotation);
}

/**
 * @brief Rotates the bits in a uint8_t to the right
 *
 * @param num pointer to the number to rotate
 * @param rotation number of rotation steps to execute
 */
void rotateRight(uint8_t* num, int rotation){
    *num = (uint8_t)(((*num) << (UINT8_T_SIZE-rotation)) | ((*num) >> rotation));
}

void bit_rotate(uint8_t* value, rot_dir_t dir, int d) {
    d = CLAMP07(d);
    switch (dir) {
        case LEFT:
            rotateLeft(value, d);
            break;
        case RIGHT:
            rotateRight(value, d);
            break;
    }
}

void bit_edit(uint8_t* value, int index, uint8_t v){
   v == 0 ? bit_unset(value, index) :  bit_set(value, index);    
}
