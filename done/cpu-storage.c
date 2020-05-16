/**
 * @file cpu-registers.c
 * @brief Game Boy CPU simulation, register part
 *
 * @author C la vie
 * @date 2020
 */

#include "error.h"
#include "cpu-storage.h" // cpu_read_at_HL
#include "cpu-registers.h" // cpu_BC_get
#include "gameboy.h" // REGISTER_START
#include "util.h"
#include "opcode.h" //instruction_direct
#include <inttypes.h> // PRIX8
#include <stdio.h> // fprintf

data_t cpu_read_at_idx(const cpu_t* cpu, addr_t addr){
     data_t result = 0;
     //TODO verifier bus_read() si il fait une erreur ou ...
     bus_read(*(cpu->bus), addr, &result);

     return result;
}

addr_t cpu_read16_at_idx(const cpu_t* cpu, addr_t addr){
    addr_t result = 0;
    bus_read16(*(cpu->bus), addr, &result);
    
    return result;
}

int cpu_write_at_idx(cpu_t* cpu, addr_t addr, data_t data){
	M_REQUIRE_NON_NULL(cpu);
	
	cpu->write_listener = addr;
	return bus_write(*(cpu->bus), addr, data);
}

int cpu_write16_at_idx(cpu_t* cpu, addr_t addr, addr_t data16){
	M_REQUIRE_NON_NULL(cpu);
	
	cpu->write_listener = addr;
	return bus_write16(*cpu->bus, addr, data16);
}

int cpu_SP_push(cpu_t* cpu, addr_t data16){
	M_REQUIRE_NON_NULL(cpu);
	
	cpu->SP = (uint16_t) (cpu->SP - 2u);
	return cpu_write16_at_idx(cpu, cpu->SP,data16);
}

addr_t cpu_SP_pop(cpu_t* cpu){
	//TODO est-ce qu'il faut un if cpu != NULL ?
    addr_t result = cpu_read16_at_idx(cpu, cpu->SP);
	cpu->SP = (uint16_t) (cpu->SP + 2u);
	
    return result;
}

int cpu_dispatch_storage(const instruction_t* lu, cpu_t* cpu){
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(lu);

    switch (lu->family) {
		
    case LD_A_BCR:
		cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_idx(cpu, cpu_BC_get(cpu)));
		break;
    
    case LD_A_CR:
		cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_idx(cpu, REGISTERS_START + cpu_reg_get(cpu, REG_C_CODE)));
        break;

    case LD_A_DER:
		cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_idx(cpu, cpu_DE_get(cpu)));
        break;

    case LD_A_HLRU:
		cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_HL(cpu));
		cpu_HL_set(cpu, cpu_HL_get(cpu) + extract_HL_increment(lu->opcode));
        break;

    case LD_A_N16R:
		cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_idx(cpu, cpu_read_addr_after_opcode(cpu)));
        break;

    case LD_A_N8R:
		cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_idx(cpu, REGISTERS_START + cpu_read_data_after_opcode(cpu)));
        break;

    case LD_BCR_A:
		cpu_write_at_idx(cpu, cpu_BC_get(cpu), cpu_reg_get(cpu, REG_A_CODE));
        break;

    case LD_CR_A:
		cpu_write_at_idx(cpu, REGISTERS_START + cpu_reg_get(cpu, REG_C_CODE), cpu_reg_get(cpu, REG_A_CODE));
        break;

    case LD_DER_A:
		cpu_write_at_idx(cpu, cpu_DE_get(cpu), cpu_reg_get(cpu, REG_A_CODE));
        break;

    case LD_HLRU_A:
    	cpu_write_at_HL(cpu, cpu_reg_get(cpu, REG_A_CODE));
		cpu_HL_set(cpu, cpu_HL_get(cpu) + extract_HL_increment(lu->opcode));
        break;

    case LD_HLR_N8:
		cpu_write_at_HL(cpu, cpu_read_data_after_opcode(cpu));
        break;

    case LD_HLR_R8:
		cpu_write_at_HL(cpu, cpu_reg_get(cpu, extract_reg(lu->opcode, 0)));
        break;

    case LD_N16R_A:
		cpu_write_at_idx(cpu, cpu_read_addr_after_opcode(cpu), cpu_reg_get(cpu, REG_A_CODE));
        break;

    case LD_N16R_SP:
		cpu_write16_at_idx(cpu, cpu_read_addr_after_opcode(cpu), cpu->SP); 
        break;

    case LD_N8R_A:
		cpu_write_at_idx(cpu, REGISTERS_START + cpu_read_data_after_opcode(cpu), cpu_reg_get(cpu, REG_A_CODE));
        break;

    case LD_R16SP_N16:
		cpu_reg_pair_SP_set(cpu, extract_reg_pair(lu->opcode), cpu_read_addr_after_opcode(cpu));
        break;

    case LD_R8_HLR:
		cpu_reg_set(cpu, extract_reg(lu->opcode, 3), cpu_read_at_HL(cpu));
        break;

    case LD_R8_N8:
		cpu_reg_set(cpu, extract_reg(lu->opcode, 3), cpu_read_data_after_opcode(cpu));
        break;

    case LD_R8_R8: {
		reg_kind s = extract_reg(lu->opcode, 0);
		reg_kind r = extract_reg(lu->opcode, 3);
		cpu_reg_set(cpu, r, cpu_reg_get(cpu, s));
    } break;

    case LD_SP_HL:
		cpu->SP = cpu_HL_get(cpu); 
        break;

    case POP_R16:
		cpu_reg_pair_set(cpu, extract_reg_pair(lu->opcode), cpu_SP_pop(cpu));
        break;

    case PUSH_R16:
		cpu_SP_push(cpu, cpu_reg_pair_get(cpu, extract_reg_pair(lu->opcode)));
        break;

    default:
        fprintf(stderr, "Unknown STORAGE instruction, Code: 0x%" PRIX8 "\n", cpu_read_at_idx(cpu, cpu->PC));
        return ERR_INSTR;
        break;
    } // switch

    return ERR_NONE;
}
