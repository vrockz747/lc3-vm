//includes
#include <signal.h> 

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

#include "memory.h"
#include "cpu.h"

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
                op_add(instr);
            }
                break;

            case OP_AND:
            {
                op_and(instr);
            }
                break;

            case OP_NOT:
            {
                op_not(instr);
            }
                break;

            case OP_BR:
            {
                op_br(instr);
            }
                break;

            case OP_JMP:
            {
                op_jmp(instr);
            }
                break;

            case OP_JSR:
            {
                op_jsr(instr);
            }
                break;

            case OP_LD:
            {
                op_ld(instr);
            }
                break;

            case OP_LDI:
            {
               op_ldi(instr);
            }
                break;

            case OP_LDR:
            {
                op_ldr(instr);
            }
                break;

            case OP_LEA:
            {
                op_lea(instr);
            }
                break;

            case OP_ST:
            {
                op_st(instr);
            }
                break;

            case OP_STI:
            {
                op_sti(instr);
            }
                break;

            case OP_STR:
            {
                op_str(instr);
            }
                break;

            case OP_TRAP:
            {
                reg[R_R7] = reg[R_PC];

                switch (instr & 0xFF)
                    {
                    case TRAP_GETC:
                    {
                        trap_getc();
                    }
                        break;

                    case TRAP_OUT:
                    {
                        trap_out();
                    }
                        break;

                    case TRAP_PUTS:
                    {
                        trap_puts();
                    }
                        break;

                    case TRAP_IN:
                    {
                        trap_in();
                    }
                        break;

                    case TRAP_PUTSP:
                    {
                        trap_putsp();
                    }
                        break;

                    case TRAP_HALT:
                    {
                        trap_halt();
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