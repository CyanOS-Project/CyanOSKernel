
#include "boot.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/paging.h"
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
	multiboot_tag_start* boot_info;
	asm volatile("mov %%ebx,%0" : "=r"(boot_info));
	uintptr_t size = boot_info->total_size;
	Memory::setup();
	initiate_console();
	uintptr_t vboot_info = (uintptr_t)Memory::map(uintptr_t(boot_info), boot_info->total_size,
	                                              MEMORY_TYPE::KERNEL | MEMORY_TYPE::WRITABLE);
	// FIXME: temp hack because map maps only aligned pages.
	vboot_info = (uintptr_t(boot_info) % PAGE_SIZE) + vboot_info;

	printf("maping %X/%X into %X\n", boot_info, size, vboot_info);
	printf("physical_add: %X\n", Paging::get_physical_page(vboot_info) * PAGE_SIZE);
	Bootloader_info bootloader_info;
	memset(&bootloader_info, 0, sizeof(Bootloader_info));
	parse_mbi((uintptr_t)vboot_info, bootloader_info);
	void* new_stack = Memory::alloc(0x4000, MEMORY_TYPE::KERNEL | MEMORY_TYPE::WRITABLE);
	SET_STACK((uint32_t)new_stack + 0x4000);
	kernel_init();
	// JMP(kernel_init);
	HLT();
	return;
}

inline uintptr_t align_to(uintptr_t size, unsigned alignment)
{
	if (size == 0)
		return alignment;
	else
		return size + alignment - (size % alignment);
}

const char* mmap_entry_type_text[] = {"Unknown"           //
                                      "AVAILABLE",        //
                                      "RESERVED",         //
                                      "ACPI_RECLAIMABLE", //
                                      "NVS",              //
                                      "BAD_RAM"};

void parse_mbi(uintptr_t multiboot_info, Bootloader_info& info)
{
	multiboot_tag* current_tag = (multiboot_tag*)(multiboot_info + sizeof(multiboot_tag_start));
	while (current_tag->type != MULTIBOOT_TAG_TYPE_END) {
		printf("checking: %x, Type: %d\n", current_tag, current_tag->type);
		switch (current_tag->type) {
			case MULTIBOOT_TAG_TYPE_MODULE: {
				multiboot_tag_module* tag = (multiboot_tag_module*)current_tag;
				printf("Module Tag:\n");
				printf("mod_start :%x\n", tag->mod_start);
				printf("mod_end :%x\n", tag->mod_end);
				printf("cmdline :%s\n", tag->cmdline);
				printf("mem_upper :%d\n", tag->size);
				info.modules[0].start = tag->mod_start;
				info.modules[0].size = tag->mod_start - tag->mod_start;
				break;
			}
			case MULTIBOOT_TAG_TYPE_MMAP: {
				multiboot_tag_mmap* tag2 = (multiboot_tag_mmap*)current_tag;
				size_t i = 0;
				while (tag2->entries[i].type != 0) {
					printf("addr :%x\n", tag2->entries[i].addr);
					printf("len :%x\n", tag2->entries[i].len);
					printf("type :%s\n", mmap_entry_type_text[tag2->entries[i].type]);
					i++;
				}
				break;
			}
		}
		current_tag = (multiboot_tag*)(uintptr_t(current_tag) + align_to(current_tag->size, MULTIBOOT_INFO_ALIGN));
	}
}