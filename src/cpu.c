#include "cpu.h"
#include "memory.h"

uint16_t reg[R_COUNT];

//sign extend
uint16_t sign_extend(uint16_t x, int bit_count)
{
    if ((x >> (bit_count - 1)) & 1) {
    x |= (0xFFFF << bit_count);
}
    return x;
}

//Swap
uint16_t swap16(uint16_t x){
       return ((x << 8) | (x >> 8));
    }

//Everytime a value writen to a(any) reg -> update flag reg
void updateflag( uint16_t r ){
    if ( reg[r] == 0 ){
        reg[R_COND] = FL_ZRO;
    }
    else if ( (reg[r] >> 15) ){
        reg[R_COND] = FL_NEG;
    }else{
        reg[R_COND] = FL_POS;
    }
}

//opcodes
void op_add(uint16_t instr){
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t r1 = (instr >> 6) & 0x7;
    uint16_t imm_flag = (instr >> 5) & 0x1;
    if (imm_flag){
    uint16_t imm5 = sign_extend(instr & 0x1F, 5);
    reg[r0] = reg[r1] + imm5;
    }
    else{
    uint16_t r2 = instr & 0x7;
    reg[r0] = reg[r1] + reg[r2];
    }
    updateflag(r0);
}

void op_and(uint16_t instr){
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t r1 = (instr >> 6 ) & 0x7;
    if( (instr >> 5) & 0x1 ){ //check for imm flag
        uint16_t imm5 = sign_extend(instr & 0x1F,5);
        reg[r0] = reg[r1] & imm5;
    }
    else{
        uint16_t r2 = instr & 0x7;
        reg[r0] = reg[r1] & reg[r2];
    }
    updateflag(r0);
}

void op_not(uint16_t instr){
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t r1 = (instr >> 6) & 0x7;
    reg[r0] = ~reg[r1];
    updateflag(r0);
}

void op_br(uint16_t instr){
    uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
    uint16_t cond_flag = (instr >> 9) & 0x7;
    if (cond_flag & reg[R_COND])
    {
    reg[R_PC] += pc_offset;
    }
}

void op_jmp(uint16_t instr){
    uint16_t r1 =  ((instr >> 6) & 0x7);
    reg[R_PC] = reg[r1];
}

void op_jsr(uint16_t instr){
    reg[R_R7] = reg[R_PC];  
    uint16_t longflag = ((instr >> 11 ) & 1);  
    if ( !longflag ){
        uint16_t r1 = (instr >> 6) & 0x7;
        reg[R_PC] = reg[r1];
    }
    else{
        uint16_t offset = sign_extend( instr & 0x7FF ,11);
        reg[R_PC] = reg[R_PC] + offset;
    }
}

void op_ld(uint16_t instr){
    uint16_t pcoffset = sign_extend( instr & 0x1FF, 9);
    uint16_t r0 = (instr >> 9) & 0x7;
    reg[r0] = mem_read(reg[R_PC] + pcoffset);
    updateflag(r0);
}

void op_ldi(uint16_t instr){
    uint16_t r0 = ( instr >> 9 ) & 0x7;
    uint16_t pcoffset9 = sign_extend(instr & 0x1FF, 9);
    reg[r0] = mem_read(mem_read(reg[R_PC]+pcoffset9));
    updateflag(r0);
}

void op_ldr(uint16_t instr){
    uint16_t r0 = (instr>>9) & 0x7;
    uint16_t r1 = (instr >> 6) & 0x7;
    uint16_t pcoffset = sign_extend( instr & 0x3F, 6);
    reg[r0] = mem_read( reg[r1] + pcoffset );
    updateflag(r0);
}

void op_lea(uint16_t instr){
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t pcoffset = sign_extend(instr & 0x1FF, 9);
    reg[r0] = reg[R_PC] + pcoffset;
    updateflag(r0);
}

void op_st(uint16_t instr){
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t pcoffset = sign_extend(instr & 0x1FF, 9);
    mem_write(reg[R_PC] + pcoffset, reg[r0]);
}

void op_sti(uint16_t instr){
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t pcoffset = sign_extend(instr & 0x1FF, 9);
    mem_write(mem_read(reg[R_PC] + pcoffset), reg[r0]);
}

void op_str(uint16_t instr){
    uint16_t pcoffset = sign_extend(instr & 0x3F, 6);
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t r1 = (instr >> 6) & 0x7;
    mem_write(reg[r1] + pcoffset, reg[r0]);
}


