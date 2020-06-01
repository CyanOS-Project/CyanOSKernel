#define JMP(x)       asm("JMP %0" : : "r"(kernel_init) :)
#define HLT()        asm("HLT")
#define SET_STACK(x) asm("MOV %0,%%ESP; PUSHL $0" : : "r"(x) :) // PUSH for the debuggers to work properly.
