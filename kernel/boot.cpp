
#include "boot.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/boot_paging.h"
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
    (uint32_t)VIR_TO_PHY(kernel_boot)

};

extern uint32_t KERNEL_STACK;

__attribute__((section(".bootstrap"))) void kernel_boot()
{
	setup_startup_paging();
	SET_STACK(&KERNEL_STACK);
	JMP(kernel_init);
	HLT();
	return;
}
