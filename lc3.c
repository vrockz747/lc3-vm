/*Things understood
Ek CPU design kar rahe hai, to basically ek cpu kya karta hai?
instruction fetch karta hai "RAM se" and usko apne definitions se compare karke
registers the value change karta hai! bas
To RAM me instrs dalne ke lie phele ROM hona chie, ROM se instrs RAM ne dalake
cpu fetch karta hai, ROM basically machine code hai, for here 16-bits instrs(0101100..~>16) ka
sets which tell the cpu what to do, to hmlog ye ROM file ko memory me dalke usse 16bits me instrs
leke emulate karnege is CPU ko,
this cpu doenst have cache memory, extra registers, more OPcode instructions etc

comp arch can be made more complex and effcient with more instr? cache mem? what else? 
whats the major diff between modern archs and this one
can this one be improved more with few more circuits?
can those ckts be emulated?
can it(new arch) then be compared with its previous arch
CAN the max possible RAM be increased with 2 sets? if the CPU somehow knows the diffs between the two
*/

//includes
#include <stdint.h>
#include <stdio.h>

//Memory
#define MEMORY_MAX (1 << 16) //macro defined for max memory
uint16_t memory[MEMORY_MAX]; //note since its a memory we need indexing therefore ofcourse its a Array

//Registers - Just storing the index of registers analogus to 
enum {  //how was this thought of??
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,   //used for extra storage? saving current PC
    R_PC, /* program counter */
    R_COND,
    R_COUNT
};
//MAKE ARRAY / STORE REGISTERS
uint16_t reg[R_COUNT]; // Note all registers are  16-bit

//Opcodes
enum {
    OP_BR = 0, //BRANCH
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

//Conditional Flags  ---experiment zone
enum {
    FL_POS = 1 << 0, // P
    FL_ZRO = 1 << 1, // Z
    FL_NEG = 1 << 2  // N
};

//TRAP Codes - defining the values to be compared with opcodes in switch cases
enum {
    TRAP_GETC  = 0x20, //gets char from keyboard, not echoed onto the terminal
    TRAP_OUT   = 0x21,  //outputs a char 
    TRAP_PUTS  = 0x22, // outputs a word string
    TRAP_IN    = 0x23,     //get char from keyboard, echoed onto the the term
    TRAP_PUTSP = 0x24,  // output a byte string
    TRAP_HALT  = 0x25    //halt the program
};


int main(int argc, const char* argv[]){

    //load argumemts
    if (argc < 2){
        //show usage
        prinf("lc3 [image-file] ... \n");
        exit(2);
    }
    //parse the CLargument
    for (int j = 1; j < argc; ++j){ //------------why ++j and not j++?
        if(!read_image(argv[j])){
            printf("failed to load image: %s\n", argv[j]);
            exit(1);
        }
    }
    /*@{setup}*/

    //since exactly one condiotion flag should be set at any given time, set the Z flag
    reg[R_COND] = FL_ZRO;

    //Set PC (program counter) to starting position
    //0x3000 is the default
    enum { PC_START = 0x3000 };
    reg[R_PC] = PC_START;

    int running = 1; //basically the on/off switch of processor
    while (running) {
        //FETCH
        uint16_t instr = mem_read(reg[R_PC]++); //instructions get loaded in mem, then fetched by CPU
        uint16_t op = instr >> 12; //16 - 4(opcode) = 12

        switch (op) //how is switch op working
        {
            case OP_ADD:
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t r1 = (instr >> 6) & 0x7;
                if ( (instr >> 5) & 0x1 ){
                    uint16_t imm5 = sign_extend(instr & 0x1F, 5);
                }
                break;

            case OP_AND:
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t r1 = (instr >> 6 ) & 0x7;
                if( (instr >> 5) & 0x1){ //check for imm flag
                    uint16_t imm5 = sign_extend(instr & 0x1F,5);
                    reg[r0] = reg[r1] & imm5;
                }else{
                    uint16_t r2 = instr & 0x7;
                    reg[r0] = reg[r1] & reg[r2];
                }
                updateflag(r0);
                break;

            case OP_NOT:
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t r1 = (instr >> 6) & 0x7;
                reg[r0] = ~reg[r1];
                updateflag(r0);
                break;

            case OP_BR:
    
                uint16_t n = (instr >> 11) & 0x1;
                uint16_t z = (instr >> 10) & 0x1;
                uint16_t p = (instr >> 9) & 0x1;
                if ((n & FL_NEG) | (z & FL_ZRO) | (p & FL_POS)){
                    uint16_t pcoffset9 = sign_extend(instr & 0x1F,9);
                    reg[R_PC] += pcoffset9;
                }
                break;

            case OP_JMP:
                uint16_t r1 =  ((instr >> 6) & 0x7);
                reg[R_PC] = reg[r1];
                break;

            case OP_JSR:
                reg[R_R7] = reg[R_PC];    
                if ( !((instr >> 11 )&0x1) ){
                    uint16_t r1 = (instr >> 6) & 0x7;
                    reg[R_PC] = reg[r1];
                }else{
                    uint16_t offset = sign_extend( instr & 0x7FF ,11);
                    reg[R_PC] = reg[R_PC] + offset;
                
                }
                break;

            case OP_LD:
                uint16_t pcoffset = sign_extend( instr & 0x1FF, 9);
                uint16_t r0 = (instr >> 9) & 0x7;
                reg[r0] = mem_read(reg[R_PC] + pcoffset);
                updateflag(r0);
                break;

            case OP_LDI: 
            //Why isnt LDI overlapping its variables with binary of the code..what if we get a memory shortage ??
            //why double address??
                uint16_t r0 = ( instr >> 9 ) & 0x7;
                uint16_t pcoffset9 = sign_extend(instr & 0x1FF, 9);
                reg[r0] = mem_read(mem_read(reg[R_PC]+pcoffset9));
                updateflag(r0);
                break;

            case OP_LDR:
                uint16_t r0 = (instr>>9) & 0x7;
                uint16_t r1 = (instr >> 6) & 0x7;
                uint16_t pcoffset = sign_extend( instr & 0x3F, 6);
                reg[r0] = mem_read( reg[r1] + pcoffset );
                updateflag(r0);
                break;

            case OP_LEA:
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t pcoffset = sign_extend(instr & 0x1FF, 9);
                reg[r0] = reg[R_PC] + pcoffset;
                updateflag(r0);
                break;

            case OP_ST://IMP - HOW WHY?
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t pcoffset = sign_extend(instr & 0x1FF, 9);
                mem_write(reg[R_PC] + pcoffset, reg[r0]);
                break;

            case OP_STI:
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t pcoffset = sign_extend(instr & 0x1FF, 9);
                mem_write(mem_read(reg[R_PC] + pcoffset), reg[r0]);
                break;

            case OP_STR:
                uint16_t pcoffset = sign_extend(instr & 0x3F, 6);
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t r1 = (instr >> 6) & 0x7;
                mem_write(reg[r1] + pcoffset, reg[r0]);
                break;

            case OP_TRAP:
            reg[R_R7] = reg[R_PC];
            switch (instr & 0xFF)
                {
                case TRAP_GETC:
                    reg[R_R0] = (uint16_t) getchar();
                    updateflag(R_R0);
                    break;

                case TRAP_OUT:
                    putc( (char)reg[R_R0] , stdout); //what happens if we remove (char)?
                    fflush(stdout);
                    break;

                case TRAP_PUTS:
                    /*  print to console a string whose 
                        starting address is stored in R7  */
                        //Note 'memory[]' is an array! 
                    uint16_t* c = memory + reg[R_R7];// pointer to a char, right? then why uint16?
                    while(*c){
                        putc( (char)*c , stdout); //needed to cast because c was uint16
                        ++c; 
                    }
                    fflush(stdout);
                    break;

                case TRAP_IN:
                    prinf("Enter a character: ");
                    reg[R_R0] = (uint16_t) getchar();
                    putc(reg[R_R0], stdout );
                    fflush(stdout);
                    updateflag(R_R0);
                    break;

                case TRAP_PUTSP:
                    uint16_t *c = memory + reg[R_R0];
                    while(*c){
                        uint8_t ch = (*c) & 0xFF; //0x8 doesnot take 8 full(1) bits, 0xFF does
                        putc( (char)ch, stdout);
                        ch = (*c)>>8;
                        if(!ch){
                            putc( (char)ch, stdout);
                        }
                        c++;
                    }
                    fflush(stdout);
                    break;

                case TRAP_HALT:
                puts("Halting");
                fflush(stdout);
                running = 0; 
                    break;
                }
                break;

            case OP_RES:
                abort();
            case OP_RTI:
                abort();
            default:
                abort();
                break;
        }
}
   /*@{SHUTDOWN}*/
}

//Everytime a value writen to a(any) reg -> update flag reg
void updateflag( uint16_t r ){
    if ( reg[r] == 0 ){
        reg[R_COND] = FL_ZRO;
    }
    else if ( reg[r] >> 15 ){  /*--------- a 1 in the left-most bit indicates negative */
        reg[R_COND] = FL_NEG;
    }else 
        reg[R_COND] = FL_POS;
}

uint16_t sign_extend(uint16_t r, int places){
    if ( ( r >> (places - 1) & 0x1 ) ){
        //negative
    }
    else {
        r = r | (1 << 16);

    }
}

//Loading program into memory


