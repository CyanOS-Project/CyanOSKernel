#define JMP(x)       asm("JMP %0" : : "r"(kernel_init) :)
#define HLT()        asm("HLT")
#define CLI()        asm("CLI")
#define STI()        asm("STI")
#define SET_STACK(x) asm("MOV %0,%%ESP; SUB $0x10,%%esp" : : "r"(x) :) // PUSH for the debuggers to work properly.
