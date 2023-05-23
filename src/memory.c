#include "memory.h"
#include "cpu.h"

//Memory Array
uint16_t memory[MEMORY_MAX];

//Loading program into memory
//making the virtual memory for VM
void read_image_file(FILE* file){

    //we take out origin to know the max_read for fread
    uint16_t origin;
    fread(&origin, sizeof(uint16_t), 1, file);
    origin = swap16(origin);

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

//Trap Routines are in Memory
void trap_getc(){
    reg[R_R0] = (uint16_t) getchar();
    updateflag(R_R0);
}

void trap_out(){
    putc( (char)reg[R_R0] , stdout);
    fflush(stdout);
}

void trap_puts(){
    /*  print to console a string whose 
    starting address is stored in R7  */
    uint16_t* c = memory + reg[R_R0];
    while(*c){
    putc( (char)*c , stdout); //needed to cast because c was uint16
    c++; 
    }
    fflush(stdout);
}

void trap_in(){
    printf("Enter a character: \n");
    reg[R_R0] = (uint16_t) getchar();
    putc(reg[R_R0], stdout );
    fflush(stdout);
    updateflag(R_R0);
}

void trap_putsp(){
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

void trap_halt(){
    puts("Halting");
    fflush(stdout);
}

