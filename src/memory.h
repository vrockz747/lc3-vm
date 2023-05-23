//Contains all the memory related declarations
#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h> //contains uint16_t
#include <stdio.h>

//TRAP Codes - defining the values to be compared with opcodes in switch cases
enum {
    TRAP_GETC  = 0x20,     // gets char from keyboard, not echoed onto the terminal
    TRAP_OUT   = 0x21,     // outputs a char 
    TRAP_PUTS  = 0x22,     // outputs a word string
    TRAP_IN    = 0x23,     // get char from keyboard, echoed onto the the term
    TRAP_PUTSP = 0x24,     // output a byte string
    TRAP_HALT  = 0x25      // halt the program
};

//Memory Mapped Registers
enum{
    MR_KBSR = 0xFE00, //kbrd status register
    MR_KBDR = 0xFE02  //kbrd data register
};

//Memory Storage
#define MEMORY_MAX (1 << 16) //macro defined for max memory
extern uint16_t memory[MEMORY_MAX];

uint16_t swap16(uint16_t x);
uint16_t check_key();

void read_image_file(FILE* file);
int read_image(const char* image_path);
void mem_write(uint16_t address, uint16_t val);
uint16_t mem_read(uint16_t address);

//Traps
void trap_getc();
void trap_out();
void trap_puts();
void trap_in();
void trap_putsp();
void trap_halt();

#endif //MEMORY_H