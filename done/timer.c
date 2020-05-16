/**
 * @file timer.c
 * @brief Game Boy Timer simulation header
 *
 * @author C la vie
 * @date 2020
 */

#include "timer.h"
#include "error.h"
#include "cpu-storage.h"
#include "timer.h"

#define ONE_CYCLE 0x4

int timer_init(gbtimer_t* timer, cpu_t* cpu) {
    M_REQUIRE_NON_NULL(timer);
    M_REQUIRE_NON_NULL(cpu);

    timer->counter = 0;
    //make a deep copy
    timer->cpu = cpu;
    //for non-pointer types
    *timer->cpu = *cpu;
    //for pointers
    timer->cpu->bus = cpu->bus;

    return ERR_NONE;
}

/**
* @brief Gets the bit at a given index
*
* @param value value to get the bit from
* @param index index of the bit
* @return returns the bit at a given index
*/
bit_t bit_get_16(uint16_t value, int index) {
    return (bit_t)((value & (0x01 << index)) >> index);
}


/**
 * @brief Calculates the state of the timer
 *
 * @param timer timer
 * @return returns the conjunction of 2 bits
 */
bit_t timer_state(gbtimer_t* timer) {
    M_REQUIRE_NON_NULL(timer);

    data_t tac = cpu_read_at_idx((const cpu_t*)timer->cpu, REG_TAC);
    bit_t bit_0_tac= bit_get(tac, 0);
    bit_t bit_1_tac= bit_get(tac, 1);
    bit_t bit_2_tac= bit_get(tac, 2);
    
    bit_t bit_main_counter = 0;
    if(bit_1_tac) bit_main_counter = bit_0_tac ? bit_get_16(timer->counter, 7) : bit_get_16(timer->counter, 5);
    else bit_main_counter = bit_0_tac ? bit_get_16(timer->counter, 3) : bit_get_16(timer->counter, 9);
    

    return (bit_2_tac && bit_main_counter);
}

/**
 * @brief Increments the secondary timer
 *
 * @param timer timer
 * @param old_state old state
 */
void timer_incr_if_state_change(gbtimer_t* timer, bit_t old_state) {
    if(old_state && !timer_state(timer)) {
        data_t second_timer_value = cpu_read_at_idx((const cpu_t*)timer->cpu, REG_TIMA);
        if(second_timer_value == 0xFF) {
            cpu_request_interrupt(timer->cpu, TIMER);
            second_timer_value = cpu_read_at_idx((const cpu_t*)timer->cpu, REG_TMA);
        } else {
            second_timer_value++;
        }
        
        cpu_write_at_idx(timer->cpu, REG_TIMA, second_timer_value);
    }
}

int timer_cycle(gbtimer_t* timer) {
    M_REQUIRE_NON_NULL(timer);

    bit_t old_state = timer_state(timer);
    timer->counter =(uint16_t) ( timer->counter + ONE_CYCLE); //TODO conversion to ‘uint16_t {aka short unsigned int}’ from ‘int’
    
    cpu_write_at_idx(timer->cpu, REG_DIV, msb8(timer->counter));
    timer_incr_if_state_change(timer,old_state);

    return ERR_NONE;
}


int timer_bus_listener(gbtimer_t* timer, addr_t addr) {
    M_REQUIRE_NON_NULL(timer);
    if(addr == REG_DIV) {
        bit_t old_state = timer_state(timer);
        timer->counter = 0u;
        timer_incr_if_state_change(timer, old_state);
    } else if (addr == REG_TAC) {
        timer_incr_if_state_change(timer,timer_state(timer));
    }

    return ERR_NONE;
}




