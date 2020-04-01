#include "cpu-registers.h"
#include "error.h"
#include "bit.h"


uint8_t cpu_reg_get(const cpu_t* cpu, reg_kind reg){
	M_REQUIRE_NON_NULL(cpu);
    switch (reg){
        case REG_B_CODE:
            return cpu->B;
        case REG_C_CODE:
            return cpu->C;
        case REG_D_CODE:
            return cpu->D;
        case REG_E_CODE:
            return cpu->E;
        case REG_H_CODE:
            return cpu->H;
        case REG_L_CODE:
            return cpu->L;
        case REG_A_CODE:
            return cpu->A;
        default:
            return 0u;
    }
}

uint16_t cpu_reg_pair_get(const cpu_t* cpu, reg_pair_kind reg){
	M_REQUIRE_NON_NULL(cpu);
    switch (reg) {
        case REG_AF_CODE:
            return cpu->AF;
        case REG_HL_CODE:
            return cpu->HL;
        case REG_BC_CODE:
            return cpu->BC;
        case REG_DE_CODE:
            return cpu->DE;
        default:
            return 0u;
    }
}

void cpu_reg_set(cpu_t* cpu, reg_kind reg, uint8_t value){
	M_REQUIRE_NON_NULL(cpu);
    switch (reg){
        case REG_B_CODE:
            cpu->B = value;
            break;
        case REG_C_CODE:
            cpu->C = value;
            break;
        case REG_D_CODE:
            cpu->D = value;
            break;
        case REG_E_CODE:
            cpu->E = value;
            break;
        case REG_H_CODE:
            cpu->H = value;
            break;
        case REG_L_CODE:
            cpu->L = value;
            break;
        case REG_A_CODE:
            cpu->A = value;
            break;
    }
}

void cpu_reg_pair_set(cpu_t* cpu, reg_pair_kind reg, uint16_t value){
    //TODO what is the default case ?
    //TODO do we have to update the individual register along with the pair ?
    M_REQUIRE_NON_NULL(cpu);
    switch (reg){
        case REG_BC_CODE:
			cpu_reg_set(cpu, REG_B_CODE, msb8(value));
			cpu_reg_set(cpu, REG_C_CODE, lsb8(value));
            cpu->BC = value;
            break;
        case REG_DE_CODE:
			cpu_reg_set(cpu, REG_D_CODE, msb8(value));
			cpu_reg_set(cpu, REG_E_CODE, lsb8(value));
            cpu->DE = value;
            break;
        case REG_HL_CODE:
			cpu_reg_set(cpu, REG_H_CODE, msb8(value));
			cpu_reg_set(cpu, REG_L_CODE, lsb8(value));
            cpu->HL = value;
            break;
        case REG_AF_CODE:
			//A changer
			cpu_reg_set(cpu, REG_A_CODE, msb8(value));
			printf ("value: %X msb: %X \n",value, cpu->A);
			value = (value >> 4)<<4; //force the 4 lsb to 0;
			cpu->F = lsb8(value);
            cpu->AF = value;
            break;
    }
}



