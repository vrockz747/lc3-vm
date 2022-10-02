//includes
#include <stdint.h>


//Memory
#define MEMORY_MAX (1 << 16) //macro of max memory
uint16_t memory[MEMORY_MAX];

//Registers
enum {
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC, /* program counter */
    R_COND,
    R_COUNT
};
//MAKE ARRAY / STORE REGISTERS
uint16_t reg[R_COUNT];

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

//Conditional Flags
enum {
    FL_POS = 1 << 0, // P
    FL_ZRO = 1 << 1, // Z
    FL_NEG = 1 << 2  // N
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
    enum { PC_START = 0x3000 }; //HERE USED AS A MACRO?
    reg[R_PC] = PC_START;

    int running = 1;
    while (running) {
        //FETCH
        uint16_t instr = mem_read(reg[R_PC]++); //FETCH INSTRUCTION AND PC++
        uint16_t op = instr >> 12; //16 - 4(opcode) = 12

        switch (op)
        {
            case OP_ADD:
                /*@{ADD}*/
                uint16_t r0 = (instr >> 9) & 0x7;

                break;
            case OP_AND:
                /*@{ADD}*/
                break;
            case OP_NOT:
                /*@{ADD}*/
                break;
            case OP_BR:
                /*@{ADD}*/
                break;
            case OP_JMP:
                /*@{ADD}*/
                break;
            case OP_JSR:
                /*@{ADD}*/
                break;
            case OP_LD:
                /*@{ADD}*/
                break;
            case OP_LDI:
                /*@{ADD}*/
                break;
            case OP_LDR:
                /*@{ADD}*/
                break;
            case OP_LEA:
                /*@{ADD}*/
                break;
            case OP_ST:
                /*@{ADD}*/
                break;
            case OP_STI:
                /*@{ADD}*/
                break;
            case OP_STR:
                /*@{ADD}*/
                break;
            case OP_TRAP:
                /*@{ADD}*/
                break;
            case OP_RES:
            case OP_RTI:
            default:
            /*@{BADOPCODE}*/
                break;
        }
   }
   /*@{SHUTDOWN}*/
}

//Everytime a value writen to a(any) reg => update flag reg
void updateflag( uint16_t r ){
    if ( reg[r] == 0 ){
        reg[R_COND] = FL_ZRO;
    }
    else if ( reg[r] >> 15 ){  /*--------- a 1 in the left-most bit indicates negative */
        reg[R_COND] = FL_NEG;
    }else 
        reg[R_COND] = FL_POS;
}







