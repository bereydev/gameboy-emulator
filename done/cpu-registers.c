#include "cpu-registers.h"
#include "error.h"

uint16_t cpu_reg_pair_get(const cpu_t* cpu, reg_pair_kind reg){
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


uint8_t cpu_reg_get(const cpu_t* cpu, reg_kind reg){
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

void cpu_reg_pair_set(cpu_t* cpu, reg_pair_kind reg, uint16_t value){
    //TODO what is the default case ?
    //TODO do we have to update the individual register along with the pair ?
    switch (reg){
        case REG_BC_CODE:
            cpu->BC = value;
            break;
        case REG_DE_CODE:
            cpu->DE = value;
            break;
        case REG_HL_CODE:
            cpu->HL = value;
            break;
        case REG_AF_CODE:
            cpu->AF = (value >> 4)<<4;//force the 4 lsb to 0;
            break;
    }
}

void cpu_reg_set(cpu_t* cpu, reg_kind reg, uint8_t value){
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

int cpu_init(cpu_t* cpu){
    //set everything to 0
    cpu->alu.flags = 0u;
    cpu->alu.flags = 0u;
    for (int i = REG_B_CODE; i <= REG_A_CODE; ++i) {
        cpu_reg_set(cpu, i, 0u);
    }
    for (int i = REG_BC_CODE; i <= REG_AF_CODE; ++i){
        cpu_reg_pair_set(cpu, i, 0u);
    }
    //TODO Pas sur que ce soit ça qu'il faut retourner
    return ERR_NONE;
}