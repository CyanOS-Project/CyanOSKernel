#define JMP(x)       asm("JMP %0" : : "r"(kernel_init) :)
#define HLT()        asm("HLT")
#define SET_STACK(x) asm("MOV %0,%%ESP" : : "r"(x) :)
