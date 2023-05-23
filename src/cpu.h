//Contains all the CPU related declarations
#ifndef CPU_H
#define CPU_H

#include "memory.h"

//CPU-Registers - Just storing the index of registers analogus-to-
//When accessing a register, the CPU uses the 'register number' to fetch the contents of that register. 
//The register number is used as an index to an internal register file, which holds the contents of all the registers
enum {
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,   // used for extra storage/ saving current PC
    R_PC,   // program counter
    R_COND,
    R_COUNT
};
//Conditional Flags
enum {
    FL_POS = 1 << 0, // P
    FL_ZRO = 1 << 1, // Z
    FL_NEG = 1 << 2  // N
};
//Opcodes
enum {
    OP_BR = 0, // BRANCH
    OP_ADD,    // ADD
    OP_LD,     // LOAD
    OP_ST,     // STORE
    OP_JSR,    // JUMP REG
    OP_AND,    // BITWISE AND
    OP_LDR,    // LOAD REGISTER
    OP_STR,    // STORE REGISTER
    OP_RTI,    // (UNUSED)
    OP_NOT,    // BITWISENOT
    OP_LDI,    // LOAD INDIRECT
    OP_STI,    // STORE INDIRECT
    OP_JMP,    // JUMP
    OP_RES,    // RESERVED (UNUSED)
    OP_LEA,    // LOAD EFFECTIVE ADDR
    OP_TRAP    // EXECUTE TRAP
};

//Set PC (program counter) to starting position
//0x3000 is the default
enum { PC_START = 0x3000 };

//MAKE ARRAY / STORE REGISTERS
extern uint16_t reg[R_COUNT];

uint16_t sign_extend(uint16_t x, int bit_count);
uint16_t swap16(uint16_t x);
void updateflag( uint16_t r );

//opcodes
void op_add(uint16_t instr);
void op_and(uint16_t instr);
void op_not(uint16_t instr);
void op_br(uint16_t instr);
void op_jmp(uint16_t instr);
void op_jsr(uint16_t instr);
void op_ld(uint16_t instr);
void op_ldi(uint16_t instr);
void op_ldr(uint16_t instr);
void op_lea(uint16_t instr);
void op_st(uint16_t instr);
void op_sti(uint16_t instr);
void op_str(uint16_t instr);

#endif //CPU_H