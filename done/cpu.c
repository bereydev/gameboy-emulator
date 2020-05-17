/**
 * @file cpu.c
 * @brief CPU model
 *
 * @author C la vie
 * @date 2020
 */

#include "error.h"
#include "opcode.h"
#include "cpu.h"
#include "component.h"
#include "cpu-alu.h"
#include <stdio.h>
#include "cpu-registers.h"
#include "cpu-storage.h"
#include <inttypes.h> // PRIX8

#define INTERRUPTS 5

typedef enum
{
    NZ,
    Z,
    NC,
    C
} cc_t;

int cpu_init(cpu_t *cpu)
{
    M_REQUIRE_NON_NULL(cpu);

    cpu->idle_time = 0u;
    cpu->PC = 0u;
    cpu->SP = 0u;
    cpu->bus = NULL;
    cpu->F = 0u;
    cpu->alu.value = 0u;
    cpu->alu.flags = 0u;
    cpu->write_listener = 0u;
    cpu->IME = 0u;
    cpu->IE = 0u;
    cpu->IF = 0u;
    cpu->HALT = 0u;

    for (int i = REG_BC_CODE; i <= REG_AF_CODE; ++i)
    {
        cpu_reg_pair_set(cpu, i, 0u);
    }
    M_EXIT_IF_ERR(component_create(&cpu->high_ram, HIGH_RAM_SIZE));

    return ERR_NONE;
}

int cpu_plug(cpu_t *cpu, bus_t *bus)
{
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(bus);

    cpu->bus = bus;
    bus_plug(*cpu->bus, &cpu->high_ram, HIGH_RAM_START, HIGH_RAM_END);

    (*cpu->bus)[REG_IE] = &cpu->IE;
    (*cpu->bus)[REG_IF] = &cpu->IF;

    return ERR_NONE;
}

void cpu_free(cpu_t *cpu)
{
    if (cpu != NULL)
    {
        bus_unplug(*cpu->bus, &cpu->high_ram);
        component_free(&cpu->high_ram);
        cpu->bus = NULL;
    }
}

/**
 * @brief Executes a jump if the condition is satisfied
 * @param flags, flags of the cpu you want to compare to
 * @param opcode, the opcode corresponding to the jump
 * @return 
 */
int is_condition(flags_t flags, opcode_t opcode)
{
    flag_bit_t c = get_C(flags);
    flag_bit_t z = get_Z(flags);
    switch (extract_cc(opcode))
    {
    case NZ:
        return !z;
    case Z:
        return z;
    case NC:
        return !c;
    case C:
        return c;
    default:
        return ERR_BAD_PARAMETER;
    }
}

/**
 * @brief Executes an instruction
 * @param lu instruction
 * @param cpu, the CPU which shall execute
 * @return error code
 *
 * See opcode.h and cpu.h
 */
static int cpu_dispatch(const instruction_t *lu, cpu_t *cpu)
{
    M_REQUIRE_NON_NULL(lu);
    M_REQUIRE_NON_NULL(cpu);

    //remet à 0 l'ALU du CPU
    cpu->alu.value = 0u;
    cpu->alu.flags = 0u;

    //exécution de l'instruction reçue
    switch (lu->family)
    {

    // ALU
    case ADD_A_HLR:
    case ADD_A_N8:
    case ADD_A_R8:
    case INC_HLR:
    case INC_R8:
    case ADD_HL_R16SP:
    case INC_R16SP:
    case SUB_A_HLR:
    case SUB_A_N8:
    case SUB_A_R8:
    case DEC_HLR:
    case DEC_R8:
    case DEC_R16SP:
    case AND_A_HLR:
    case AND_A_N8:
    case AND_A_R8:
    case OR_A_HLR:
    case OR_A_N8:
    case OR_A_R8:
    case XOR_A_HLR:
    case XOR_A_N8:
    case XOR_A_R8:
    case CPL:
    case CP_A_HLR:
    case CP_A_N8:
    case CP_A_R8:
    case SLA_HLR:
    case SLA_R8:
    case SRA_HLR:
    case SRA_R8:
    case SRL_HLR:
    case SRL_R8:
    case ROTCA:
    case ROTA:
    case ROTC_HLR:
    case ROT_HLR:
    case ROTC_R8:
    case ROT_R8:
    case SWAP_HLR:
    case SWAP_R8:
    case BIT_U3_HLR:
    case BIT_U3_R8:
    case CHG_U3_HLR:
    case CHG_U3_R8:
    case LD_HLSP_S8:
    case DAA:
    case SCCF:
        M_EXIT_IF_ERR(cpu_dispatch_alu(lu, cpu));
        break;

    // STORAGE
    case LD_A_BCR:
    case LD_A_CR:
    case LD_A_DER:
    case LD_A_HLRU:
    case LD_A_N16R:
    case LD_A_N8R:
    case LD_BCR_A:
    case LD_CR_A:
    case LD_DER_A:
    case LD_HLRU_A:
    case LD_HLR_N8:
    case LD_HLR_R8:
    case LD_N16R_A:
    case LD_N16R_SP:
    case LD_N8R_A:
    case LD_R16SP_N16:
    case LD_R8_HLR:
    case LD_R8_N8:
    case LD_R8_R8:
    case LD_SP_HL:
    case POP_R16:
    case PUSH_R16:
        M_EXIT_IF_ERR(cpu_dispatch_storage(lu, cpu));
        break;

    // JUMP
    case JP_CC_N16:
        if (is_condition(cpu->F, lu->opcode))
        {
            cpu->PC = cpu_read_addr_after_opcode(cpu) - lu->bytes;
            cpu->idle_time += lu->xtra_cycles;
        }
        break;

    case JP_HL:
        cpu->PC = cpu->HL - lu->bytes;
        break;

    case JP_N16:
        cpu->PC = cpu_read_addr_after_opcode(cpu) - lu->bytes;
        break;

    case JR_CC_E8:
        if (is_condition(cpu->F, lu->opcode))
        {
            cpu->PC += (signed char)cpu_read_data_after_opcode(cpu);
            cpu->idle_time += lu->xtra_cycles;
        }
        break;

    case JR_E8:
        cpu->PC += (signed char)cpu_read_data_after_opcode(cpu);
        break;

    // CALLS
    case CALL_CC_N16:
        if (is_condition(cpu->F, lu->opcode))
        {
            M_EXIT_IF_ERR(cpu_SP_push(cpu, cpu->PC + lu->bytes));
            cpu->PC = cpu_read_addr_after_opcode(cpu) - lu->bytes;
            cpu->idle_time += lu->xtra_cycles;
        }
        break;

    case CALL_N16:
        M_EXIT_IF_ERR(cpu_SP_push(cpu, cpu->PC + lu->bytes));
        cpu->PC = cpu_read_addr_after_opcode(cpu) - lu->bytes;
        break;

    // RETURN (from call)
    case RET:
        cpu->PC = cpu_SP_pop(cpu) - lu->bytes;
        break;

    case RET_CC:
        if (is_condition(cpu->F, lu->opcode))
        {
            cpu->PC = cpu_SP_pop(cpu) - lu->bytes;
            cpu->idle_time += lu->xtra_cycles;
        }
        break;

    case RST_U3:
        M_EXIT_IF_ERR(cpu_SP_push(cpu, cpu->PC + lu->bytes));
        cpu->PC = (extract_n3(lu->opcode) << 3u) - lu->bytes; //n3 * 8
        break;

    // INTERRUPT & MISC.
    case EDI:
        cpu->IME = extract_ime(lu->opcode);
        break;

    case RETI:
        cpu->IME = 1;
        cpu->PC = cpu_SP_pop(cpu) - lu->bytes;
        break;

    case HALT:
        cpu->HALT = 1;
        break;

    case STOP:
    case NOP:
        // ne rien faire
        break;

    default:
    {
        fprintf(stderr, "Unknown instruction, Code: 0x%" PRIX8 "\n", cpu_read_at_idx(cpu, cpu->PC));
        return ERR_INSTR;
    }
    break;

    } // switch

    //met à jour l'idle time et le PC
    cpu->PC += lu->bytes;
    cpu->idle_time += lu->cycles - 1;
    return ERR_NONE;
}

/**
 * @brief 
 * 
 * @param active_interrupt
 * 
 * @return 
 */

interrupt_t get_interrupt_number(uint8_t active_interrupt)
{
    for (int i = 0; i < INTERRUPTS; i++)
    {
        if (bit_get(active_interrupt, i))
            return i;
    }
    //interrupt_t est unsigned
    return 5;
}

/**
 * @brief Obtains the next instruction to execute and calls cpu_dispatch()
 * 
 * @param cpu, the CPU which shall run
 * 
 * @return error code
 */
static int cpu_do_cycle(cpu_t *cpu)
{
    M_REQUIRE_NON_NULL(cpu);

    uint8_t active_interrupts = cpu->IE & cpu->IF;
    if (cpu->IME && (active_interrupts))
    {
        cpu->IME = 0;
        interrupt_t interrupt_to_handle = get_interrupt_number(active_interrupts);
        //interrupt_t est unsigned
        if (interrupt_to_handle <= 5u)
        {
            bit_unset(&cpu->IF, interrupt_to_handle);
            M_EXIT_IF_ERR(cpu_SP_push(cpu, cpu->PC));
            cpu->PC = 0x40 + (interrupt_to_handle << 3u);
            cpu->idle_time += 5u;
        }
        else
        {
            return ERR_BAD_PARAMETER;
        }
    }

    data_t byte_at_PC = cpu_read_at_idx(cpu, cpu->PC);
    instruction_t instruction = byte_at_PC == PREFIXED ? instruction_prefixed[cpu_read_data_after_opcode(cpu)] : instruction_direct[byte_at_PC];
    M_EXIT_IF_ERR(cpu_dispatch(&instruction, cpu));

    return ERR_NONE;
}

int cpu_cycle(cpu_t *cpu)
{
    M_REQUIRE_NON_NULL(cpu);

    cpu->write_listener = 0;

    if (cpu->idle_time > 0u)
    {
        cpu->idle_time--;
    }
    else
    {
        if (cpu->HALT && (cpu->IF & cpu->IE))
        {
            cpu->HALT = 0;
            cpu_do_cycle(cpu);
        }
        else if (!cpu->HALT)
        {
            cpu_do_cycle(cpu);
        }
    }

    return ERR_NONE;
}

void cpu_request_interrupt(cpu_t *cpu, interrupt_t i)
{
    bit_set(&cpu->IF, i);
}
