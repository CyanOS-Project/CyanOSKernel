
#include "boot.h"
#include "Arch/x86/asm.h"
#include "Devices/Console/console.h"
#include "VirtualMemory/memory.h"
#include "kernel_init.h"
#include "kernel_map.h"

__attribute__((section(".multiboot2"))) const volatile Mutiboot2_Header my_multiboot2_header = {
    .header = {.magic = MULTIBOOT2_HEADER_MAGIC,
               .architecture = MULTIBOOT_ARCHITECTURE_I386,
               .header_length = sizeof(Mutiboot2_Header),
               .checksum = MULTIBOOT2_HEADER_CHECKSUM},
    .address = {.type = MULTIBOOT_HEADER_TAG_ADDRESS,
                .flags = 0,
                .size = sizeof(multiboot_header_tag_address),
                .header_addr = VIR_TO_PHY((uint32_t)&my_multiboot2_header),
                .load_addr = KERNEL_PHYSICAL_ADDRESS,
                .load_end_addr = 0,
                .bss_end_addr = 0},
    .entry = {.type = MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS,
              .flags = 0,
              .size = sizeof(multiboot_header_tag_entry_address),
              .entry_addr = VIR_TO_PHY((uint32_t)kernel_boot)},
    .end = {.type = MULTIBOOT_HEADER_TAG_END, //
            .flags = 0,
            .size = sizeof(multiboot_header_tag)},
};

__attribute__((section(".bootstrap_stack"))) volatile char boostrap_stack[0x1000];
__attribute__((section(".bootstrap"))) void kernel_boot()
{

	SET_STACK(VIR_TO_PHY((uint32_t)boostrap_stack) + sizeof(boostrap_stack));
	Memory::setup();
	MultibootInfo* boot_info;
	asm volatile("mov %%ebx,%0" : "=r"(boot_info));
	MultibootInfo* boot = (MultibootInfo*)Memory::map(uintptr_t(boot_info), sizeof(MultibootInfo),
	                                                  MEMORY_TYPE::KERNEL | MEMORY_TYPE::WRITABLE);
	initiate_console();
	printf("%x", boot->addr);
	void* new_stack = Memory::alloc(0x4000, MEMORY_TYPE::KERNEL | MEMORY_TYPE::WRITABLE);
	SET_STACK((uint32_t)new_stack + 0x4000);
	JMP(kernel_init);
	HLT();
	return;
}
