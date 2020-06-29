
#include "boot.h"
#include "Arch/x86/asm.h"
#include "VirtualMemory/memory.h"
#include "kernel_init.h"
#include "kernel_map.h"

__attribute__((section(".boot"))) const volatile multiboot_header my_multiboot_header = { //
    0x1BADB002,                                                                           //
    0x10003,                                                                              //
    (uint32_t) - (0x10003 + 0x1BADB002),
    (uint32_t)VIR_TO_PHY((uint32_t)(&my_multiboot_header)),
    KERNEL_PHYSICAL_ADDRESS,
    0,
    0,
    (uint32_t)VIR_TO_PHY((uint32_t)kernel_boot)

};

extern uint32_t BOOTSTARP_STACK_END;
__attribute__((section(".bootstrap_stack"))) volatile char boostrap_stack[0x1000];
__attribute__((section(".bootstrap"))) void kernel_boot()
{
	SET_STACK(VIR_TO_PHY((uint32_t)boostrap_stack) + sizeof(boostrap_stack));
	Memory::setup();
	void* new_stack = Memory::alloc(0x4000, MEMORY_TYPE::KERNEL | MEMORY_TYPE::WRITABLE);
	SET_STACK((uint32_t)new_stack + 0x4000);
	JMP(kernel_init);
	HLT();
	return;
}
