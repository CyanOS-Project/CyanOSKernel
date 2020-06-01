
#include "boot.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/paging.h"
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

extern uint32_t PAGE_TABLES_END;

__attribute__((section(".bootstrap"))) void kernel_boot()
{
	SET_STACK(VIR_TO_PHY((uint32_t)&PAGE_TABLES_END));
	setup_paging();
	SET_STACK((uint32_t)&PAGE_TABLES_END);
	JMP(kernel_init);
	HLT();
	return;
}
