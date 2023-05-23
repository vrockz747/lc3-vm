## LC3-Virtual Machine

An Emulator for the LC3 Architecture, which can run any program designed for the 
lc3-computer.

### Resources/Reference:

[Write your Own Virtual Machine](https://www.jmeiners.com/lc3-vm/) ( Guide to lc3 )  
[lc3tools](https://highered.mheducation.com/sites/0072467509/student_view0/lc-3_simulator.html) ( Simulator, Assembler etc for lc3 )  
[lc3-ISA](https://www.jmeiners.com/lc3-vm/supplies/lc3-isa.pdf) ( ISA for lc3 )  

## Usage:

**(For UNIX-based OS)**

**Build the VM:**

```
$ make
```

**Define the target .obj file**,
     "tests/games" for example:

```
$ lc3vm tests/games/2048.obj
```

**Delete unecessary .o files:**
```
$ make clean
```
**Run the programs**

-----

# LC3 Architecture:

### Address Space : 

```
2^16 (65536 locations)  
16-bits wide  
```

### Register Set: 

```
8 GPR numbered from 000 (0) - 111 (7)  
Each 16 bits wide  
N-P-Z, 3 bit conditional register  
Program-Counter(PC)  
```

### Data Types: 

```
16 bits 2's complement integer
```

# Opcodes

## Operate Instructions: 

```
ADD, AND, NOT
```

## Data Movement: 

```
LD, LDI, LDR, LEA, ST, STR, STI
```

### Addressing Modes:

| PC-Relative                                               | Base+Offset9                                                 | R_Base+Offset6                                               |
| --------------------------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| **LD(Load)<br />XXXX DST --OFF9---**                      | **LDI(Load Indirect)<br />XXXX DST --OFF9---**               | **LDR(Load Register)<br />XXXX DST BSE -OFF6-**              |
| 16-4+3=9bits<br />Used as *Signed* Offset from current PC | Loads the data indirectly by <br />PC + OFF9 = addr of 16bitaddr | Register as Base<br />Use a *register* to generate full 16bit addr |
| Limited Range to PC+/- 256                                | Can Indirectly generate a full 16 bit address                | Base register contains a 16 bit addr + off6 = req addr       |

### LEA : (load effective address)  

Computes address like PC-relative (PC + signed offset) and stores the result into a register.

> **To Note**: Indirection or segmentation can be used beyond the limitations of the ISA.   These techniques involve using additional registers or complex memory management mechanisms.

## Control: 

```
BR, JSR/JSRR, JMP, RTI, TRAP
```

### **Used to Alter the seq of instr (PC)**

### Condition codes: N P Z

> Load instructions (**LD, LDI, LDR**, and **LEA**) and   
> Operate instructions (**ADD, AND**, and **NOT**)   
>
> > **each *load* a result into one of the eight GPR.**  
> > All other LC-3 instructions leave the condition codes unchanged

Set based on whether that result, is N, P or Z.


## TRAP:

Do not actually introduce any new functionality to the LC-3.  
Make it easier to program by packaging common *sys calls* tasks into one trap code.  
These routines executed by the OS.

## Memory Mapped I/O:

Special Locations are designated in memory to handle I/O  
To writing to these mapped registers is to write in their designated memory locations.

```assembly
                        MEMORY MAP OF LC3

            +--------------------------------------+	 x0000
            |           Trap Vector Table          |     
            |                               	   |     x00FF
            +--------------------------------------+	 x0100
            |        Interrupt Vector Table        |
            +--------------------------------------+	 x01FF
            |          				   |	 x0200
            |		  Operating System and	   |
            |		    Supervisor Stack       |
            |                                      |	 x2FFF
            +--------------------------------------+	 x3000
            |                                      |
            |      Available for user programs     |
            |                                      |
            +--------------------------------------+	 xFDFF
            |                                      |	 xFE00
            |       Device register addresses      |
            |                                      |
            +--------------------------------------+	 xFFFF

```

> The memory is mapped by BIOS or Firmware during boot initialization stage.

## General Abstraction of Components:

The **CPU** has **Ingredients** called Opcodes to act-on/manipulate  
on the input information/data its given

```assembly
							Memory  
                    +-----+    Fetching Instr    | ...		   |  
   	Input --->  | CPU |<---------------------| program	   |  
                    +-----+		  	 | ...		   |  
                    |--------------------------->| Output	   |  
```

A **Program** is **recipie** which **instructs** or tells the CPU ***How-To*** act-on the input.  
Its made-up of only the **ingredients** the particular CPU has.  
Its basically the Algorithm we want our input to go through.  

```assembly
                +------------------------+
                |   ...      		 |
                |   AND R4, R5, R6       |		Assembler
                |   NOT R7, R8 	         |-->(to machine code)-->Instr
                |	...		 |
                |   LDR R9, LABEL        |
                |   STR R10, R11, #10 	 |
                |	...		 |
                +------------------------+
			(recipe).asm
```

**Memory** is where the Program is stored, and is accessed in n-bits chunks of array where n    is defined by the max possible addressable address for the CPU.

```assembly
 Address <---------16-bits-wide---------->
...
| x1234 | 0|1|0|1|0|1|0|0|1|1|0|0|1|1|1|0 |
...					  |
| x5678 | 1|1|1|0|0|0|1|1|0|0|1|1|0|0|1|0 | <-----Program-Counter(PC)
...                        		  |
| x9ABC | 1|0|1|0|1|0|1|0|1|0|1|0|1|0|1|0 |
...
```

----


