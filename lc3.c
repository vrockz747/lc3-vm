//includes
#include <stdint.h> 
#include <stdio.h>
#include <signal.h> 

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

//Memory Mapped Registers
enum{
    MR_KBSR = 0xFE00, //kbrd status register
    MR_KBDR = 0xFE02  //kbrd data register
};

//TRAP Codes - defining the values to be compared with opcodes in switch cases
enum {
    TRAP_GETC  = 0x20,     // gets char from keyboard, not echoed onto the terminal
    TRAP_OUT   = 0x21,     // outputs a char 
    TRAP_PUTS  = 0x22,     // outputs a word string
    TRAP_IN    = 0x23,     // get char from keyboard, echoed onto the the term
    TRAP_PUTSP = 0x24,     // output a byte string
    TRAP_HALT  = 0x25      // halt the program
};

//Memory Storage
//memory max should be 0xFFFF, how (1 << 16)???
#define MEMORY_MAX (1 << 16) //macro defined for max memory
uint16_t memory[MEMORY_MAX]; //note since its a memory we need indexing therefore ofcourse its a Array

//Registers - Just storing the index of registers analogus-to-
//When accessing a register, the CPU uses the 'register number' to fetch the contents of that register. 
//The register number is used as an index to an internal register file, which holds the contents of all the registers

enum {  // how was this thought of??
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,   // used for extra storage? saving current PC
    R_PC,   // program counter
    R_COND,
    R_COUNT
};
//MAKE ARRAY / STORE REGISTERS
uint16_t reg[R_COUNT]; // Note all registers are  16-bit

//Input Buffering
struct termios original_tio;

void disable_input_buffering(){
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}
void restore_input_buffering(){
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}
uint16_t check_key(){
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}
//handle interrupt
void handle_interrupt(int signal){
    restore_input_buffering();
    printf("\n");
    exit(-2);
}

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

//Conditional Flags
enum {
    FL_POS = 1 << 0, // P
    FL_ZRO = 1 << 1, // Z
    FL_NEG = 1 << 2  // N
};

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

//Loading program into memory
//making the virtual memory for VM
void read_image_file(FILE* file){

    //we take out origin to know the max_read for fread
    uint16_t origin;
    fread(&origin, sizeof(uint16_t), 1, file);
    origin = swap16(origin);

    //??????Shouldnt instead of max read we should put { max_read / sizeof(uint16_t) }
    uint16_t max_read = MEMORY_MAX - origin;
    //storing the file in exact location in memory as specified by arch
    uint16_t* p = memory + origin;
    size_t read = fread(p, sizeof(uint16_t), (max_read / sizeof(uint16_t)), file);

    //swapping each element to 
    while(read-- > 0){
        *p = swap16(*p);
        p++;
    }
}

//Take path of img file and send it for reading
int read_image(const char* image_path){
    FILE* file = fopen(image_path, "rb");
    if (!file){
        return 0;
    }
    read_image_file(file);
    fclose(file);
    return 1;
}

//Memory Access

void mem_write(uint16_t address, uint16_t val){
    memory[address] = val;
}

uint16_t mem_read(uint16_t address){

    /* we have memory mapped registers 
    --> we need to check if the value we want to 
    read is maybe that of memory mapped(special) reg: */  
    
    if( address == MR_KBSR){
        if(check_key()){
            memory[MR_KBSR] = (1 << 15); //because lc3 is big endian
            memory[MR_KBDR] = getchar();
        }
        else{
            memory[MR_KBSR] = 0;
        }
    }

    //if not simple return the address
    return memory[address];
}

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

int main(int argc, const char* argv[]){

    argc = 2;
    //**Load argumemts
    if (argc < 2){
        //show usage
        printf("lc3 [image-file] ... \n");
        exit(2);
    }
    //parse the CLargument
    for (int j = 1; j < argc; j++){
        if(!read_image(argv[j])){
            printf("failed to load image: %s\n", argv[j]);
            exit(1);
        }
    }
    
    //**Setup
    signal(SIGINT, handle_interrupt);
    disable_input_buffering();

    //setting the initial codn flag
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

        switch (op)
        {
            case OP_ADD:
            {
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t r1 = (instr >> 6) & 0x7;
                uint16_t imm_flag = (instr >> 5) & 0x1;
                if (imm_flag)
                {
                uint16_t imm5 = sign_extend(instr & 0x1F, 5);
                reg[r0] = reg[r1] + imm5;
                }
                else
                {
                uint16_t r2 = instr & 0x7;
                reg[r0] = reg[r1] + reg[r2];
                }
                updateflag(r0);
            }
                break;

            case OP_AND:
                {
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
                }
                break;

            case OP_NOT:
            {
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t r1 = (instr >> 6) & 0x7;
                reg[r0] = ~reg[r1];
                updateflag(r0);
            }
                break;

            case OP_BR:
            {
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                uint16_t cond_flag = (instr >> 9) & 0x7;
                if (cond_flag & reg[R_COND])
                {
                reg[R_PC] += pc_offset;
                }
            }
                break;

            case OP_JMP:
            {
                uint16_t r1 =  ((instr >> 6) & 0x7);
                reg[R_PC] = reg[r1];
            }
                break;

            case OP_JSR:
            {
                reg[R_R7] = reg[R_PC];  
                uint16_t longflag = ((instr >> 11 ) & 1);  
                if ( !longflag ){
                    uint16_t r1 = (instr >> 6) & 0x7;
                    reg[R_PC] = reg[r1];
                }else{
                    uint16_t offset = sign_extend( instr & 0x7FF ,11);
                    reg[R_PC] = reg[R_PC] + offset;
                }
            }
                break;

            case OP_LD:
            {
                uint16_t pcoffset = sign_extend( instr & 0x1FF, 9);
                uint16_t r0 = (instr >> 9) & 0x7;
                reg[r0] = mem_read(reg[R_PC] + pcoffset);
                updateflag(r0);
            }
                break;

            case OP_LDI:
            {
                uint16_t r0 = ( instr >> 9 ) & 0x7;
                uint16_t pcoffset9 = sign_extend(instr & 0x1FF, 9);
                reg[r0] = mem_read(mem_read(reg[R_PC]+pcoffset9));
                updateflag(r0);
            }
                break;

            case OP_LDR:
            {
                uint16_t r0 = (instr>>9) & 0x7;
                uint16_t r1 = (instr >> 6) & 0x7;
                uint16_t pcoffset = sign_extend( instr & 0x3F, 6);
                reg[r0] = mem_read( reg[r1] + pcoffset );
                updateflag(r0);
            }
                break;

            case OP_LEA:
            {
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t pcoffset = sign_extend(instr & 0x1FF, 9);
                reg[r0] = reg[R_PC] + pcoffset;
                updateflag(r0);
            }
                break;

            case OP_ST:
            {
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t pcoffset = sign_extend(instr & 0x1FF, 9);
                mem_write(reg[R_PC] + pcoffset, reg[r0]);
            }
                break;

            case OP_STI:
            {
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t pcoffset = sign_extend(instr & 0x1FF, 9);
                mem_write(mem_read(reg[R_PC] + pcoffset), reg[r0]);
            }
                break;

            case OP_STR:
            {
                uint16_t pcoffset = sign_extend(instr & 0x3F, 6);
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t r1 = (instr >> 6) & 0x7;
                mem_write(reg[r1] + pcoffset, reg[r0]);
            }
                break;

            case OP_TRAP:
            {
                reg[R_R7] = reg[R_PC];
                switch (instr & 0xFF)
                    {
                    case TRAP_GETC:
                    {
                        reg[R_R0] = (uint16_t) getchar();
                        updateflag(R_R0);
                    }
                        break;

                    case TRAP_OUT:
                    {
                        putc( (char)reg[R_R0] , stdout);
                        fflush(stdout);
                    }
                        break;

                    case TRAP_PUTS:
                    {
                        /*  print to console a string whose 
                            starting address is stored in R7  */
                            //Note 'memory[]' is an array! 
                        uint16_t* c = memory + reg[R_R0];
                        while(*c){
                            putc( (char)*c , stdout); //needed to cast because c was uint16
                            c++; 
                        }
                        fflush(stdout);
                    }
                        break;

                    case TRAP_IN:
                    {
                        printf("Enter a character: \n");
                        reg[R_R0] = (uint16_t) getchar();
                        putc(reg[R_R0], stdout );
                        fflush(stdout);
                        updateflag(R_R0);
                    }
                        break;

                    case TRAP_PUTSP:
                    {
                        uint16_t *c = memory + reg[R_R0];
                        while(*c){
                            uint8_t ch = (*c) & 0xFF;
                            putc( (char)ch, stdout);
                            ch = (*c)>>8;
                            if(!ch){
                                putc( (char)ch, stdout);
                            }
                            c++;
                        }
                        fflush(stdout);
                    }
                        break;

                    case TRAP_HALT:
                    {
                    puts("Halting");
                    fflush(stdout);
                    running = 0;
                    }
                        break;
                    }
                break;
            }

            case OP_RES:
                abort();
            case OP_RTI:
                abort();
            default:
                abort();
                break;
        }
}
   //shutdown
    restore_input_buffering();
}