#include <stdint.h>   // for uint8_t and uint16_t types
#include "bit.h"
// ======================================================================
/**
 * @brief a type to represent 1 single bit.
 */
/* Nous vous fournission ici un type à n'utiliser QUE lorsque vous
 * voulez représenter UN SEUL bit ; p.ex. :
 *     bit_t un_bit_tout_seul = 1;
 */
typedef uint8_t bit_t;

//longueur d'un uint8_t
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
    return (value & (0x01 << index)) >> index;
}
void bit_set(uint8_t* value, int index){
    index = CLAMP07(index);
    *value = (*value) | (0x01 << index);
}
void bit_unset(uint8_t* value, int index){
    index = CLAMP07(index);
    *value = (*value) & ~(0x01 << index);
}

const uint16_t mask_lsb8 = 0x00ff;
const uint16_t mask_msb8 = 0xff00;

uint8_t lsb8(uint16_t value){
    return value & mask_lsb8;
}
uint8_t msb8(uint16_t value){
    return (value & mask_msb8) >> 8;
}
uint8_t merge4(uint8_t value, uint8_t value2){
    return lsb4(value) | (lsb4(value2)<<4);
}
uint16_t merge8(uint8_t value, uint8_t value2){
    return value | (value2 << 8);
}

/**
 * Function to rotate bits of a number to left.
 *
 * @num         Number to rotate.
 * @rotation    Number of times to rotate left.
 */
void rotateLeft(uint8_t* num, int rotation){
    *num = ((*num) >> UINT8_T_SIZE-rotation) | ((*num) << rotation);
}

/**
 * Function to rotate bits of a number to right.
 *
 * @num         Number to rotate.
 * @rotation    Number of times to rotate right.
 */
void rotateRight(uint8_t* num, int rotation){
        *num = ((*num) << UINT8_T_SIZE-rotation) | ((*num) >> rotation);
}

void bit_rotate(uint8_t* value, rot_dir_t dir, int d){
    d = CLAMP07(d);
    if (dir == LEFT)
        rotateLeft(value, d);
    if (dir == RIGHT)
        rotateRight(value, d);
    //do we have to consider a default/error case ? do we have to check the validity of the different parameters
}

void bit_edit(uint8_t* value, int index, uint8_t v){
    //TODO
}