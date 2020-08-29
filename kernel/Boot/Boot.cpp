
#include "Boot.h"
#include "Devices/DebugPort/Logger.h"
#include "Utils/Algorithms.h"

extern uint32_t KERNEL_END;
extern uint32_t KERNEL_START;
BootloaderInfo bootloader_info;
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
                .bss_end_addr = VIR_TO_PHY(uint32_t(&KERNEL_END))},
    .entry = {.type = MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS,
              .flags = 0,
              .size = sizeof(multiboot_header_tag_entry_address),
              .entry_addr = VIR_TO_PHY((uint32_t)&kernel_boot_stage1)},
    .mod_align = {.type = MULTIBOOT_HEADER_TAG_MODULE_ALIGN,
                  .flags = 0,
                  .size = sizeof(multiboot_header_tag_module_align)},
    .end = {.type = MULTIBOOT_HEADER_TAG_END, //
            .flags = 0,
            .size = sizeof(multiboot_header_tag)},
};
extern "C" void kernel_boot_stage2(uint32_t magic, multiboot_tag_start* boot_info)
{

	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
		HLT();

	BootloaderInfo bootloader_info_local = parse_mbi((uintptr_t)boot_info);
	Memory::setup();
	// maping ramdisk to virtual memory
	uintptr_t ustar_fs = reinterpret_cast<uintptr_t>(Memory::map(bootloader_info_local.ramdisk.start, //
	                                                             bootloader_info_local.ramdisk.size,  //
	                                                             MEMORY_TYPE::KERNEL | MEMORY_TYPE::WRITABLE));
	bootloader_info_local.ramdisk.start = ustar_fs;
	memcpy(&bootloader_info, &bootloader_info_local, sizeof(BootloaderInfo));
	// Set new stack for high memory kernel.
	uintptr_t new_stack = uintptr_t(Memory::alloc(STACK_SIZE, MEMORY_TYPE::KERNEL | MEMORY_TYPE::WRITABLE));
	SET_STACK(new_stack + STACK_SIZE);
	CALL(kernel_init, &bootloader_info)
	ASSERT_NOT_REACHABLE();
	return;
}

const char* mmap_entry_type_text[] = {"Unknown"           //
                                      "AVAILABLE",        //
                                      "RESERVED",         //
                                      "ACPI_RECLAIMABLE", //
                                      "NVS",              //
                                      "BAD_RAM"};

BootloaderInfo parse_mbi(uintptr_t multiboot_info)
{
	BootloaderInfo bootloader_info_local;

	multiboot_tag* current_tag = (multiboot_tag*)(multiboot_info + sizeof(multiboot_tag_start));
	while (current_tag->type != MULTIBOOT_TAG_TYPE_END) {
		switch (current_tag->type) {
			case MULTIBOOT_TAG_TYPE_MODULE: {
				multiboot_tag_module* tag = (multiboot_tag_module*)current_tag;
				// if (strcmp(tag->cmdline, "ramdisk") == 0) {
				bootloader_info_local.ramdisk.start = tag->mod_start;
				bootloader_info_local.ramdisk.size = tag->mod_end - tag->mod_start;
				//}
				break;
			}
			case MULTIBOOT_TAG_TYPE_MMAP: {
				multiboot_tag_mmap* tag2 = (multiboot_tag_mmap*)current_tag;
				size_t i = 0;
				// info() << "Memory Map:\n";
				while (tag2->entries[i].type != 0) {
					// info() << "Addr: " << Hex(tag2->entries[i].addr) << " Size: " << tag2->entries[i].len
					//       << " Type: " << mmap_entry_type_text[tag2->entries[i].type] << "\n";
					i++;
				}
				break;
			}
		}
		current_tag = reinterpret_cast<multiboot_tag*>(uintptr_t(current_tag) +
		                                               align_to(current_tag->size, MULTIBOOT_INFO_ALIGN));
	}
	return bootloader_info_local;
}
