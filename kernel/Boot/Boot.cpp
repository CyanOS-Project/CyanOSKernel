
#include "Boot.h"
#include "Devices/DebugPort/Logger.h"
#include <Algorithms.h>

BootloaderInfo bootloader_info;
__attribute__((section(".multiboot2"))) static const volatile Mutiboot2_Header my_multiboot2_header = {
    .header = {.magic = MULTIBOOT2_HEADER_MAGIC,
               .architecture = MULTIBOOT_ARCHITECTURE_I386,
               .header_length = sizeof(Mutiboot2_Header),
               .checksum = MULTIBOOT2_HEADER_CHECKSUM},
    .address = {.type = MULTIBOOT_HEADER_TAG_ADDRESS,
                .flags = 0,
                .size = sizeof(multiboot_header_tag_address),
                .header_addr = VIR_TO_PHY((uint32_t)&my_multiboot2_header),
                .load_addr = unsigned(-1),
                .load_end_addr = 0,
                .bss_end_addr = VIR_TO_PHY(uint32_t(&KERNEL_END))},
    .entry = {.type = MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS,
              .flags = 0,
              .size = sizeof(multiboot_header_tag_entry_address),
              .entry_addr = VIR_TO_PHY((uint32_t)&_kernel_boot_stage1)},
    .mod_align = {.type = MULTIBOOT_HEADER_TAG_MODULE_ALIGN,
                  .flags = 0,
                  .size = sizeof(multiboot_header_tag_module_align)},
    .end = {.type = MULTIBOOT_HEADER_TAG_END, //
            .flags = 0,
            .size = sizeof(multiboot_header_tag)},
};

const char* mmap_entry_type_text[] = {"UNKNOWN"           //
                                      "AVAILABLE",        //
                                      "RESERVED",         //
                                      "ACPI_RECLAIMABLE", //
                                      "NVS",              //
                                      "BAD_RAM"};

extern "C" void kernel_boot_stage2(uint32_t magic, multiboot_tag_start* boot_info)
{
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
		HLT();
	volatile uintptr_t tags_physical_addr = uintptr_t(boot_info);
	volatile size_t tags_size = boot_info->total_size;

	Memory::setup();
	Logger::init();

	multiboot_tag_start* tags_addr = (multiboot_tag_start*)Memory::map(tags_physical_addr, tags_size, PAGE_READWRITE);
	parse_mbi(tags_addr);

	uintptr_t new_stack = uintptr_t(valloc(STACK_SIZE, PAGE_READWRITE));
	SET_STACK(new_stack + STACK_SIZE);
	CALL(kernel_init, &bootloader_info)

	ASSERT_NOT_REACHABLE();
	return;
}

void parse_mbi(multiboot_tag_start* multiboot_info)
{

	info() << "Kernel Start: " << Hex(uintptr_t(&KERNEL_START));
	info() << "Kernel End  : " << Hex(uintptr_t(&KERNEL_END));
	info() << "Kernel Aligned End  : " << Hex(align_to(VIR_TO_PHY(uint32_t(&KERNEL_END)), PAGE_SIZE));
	info() << "Constructors Array Start: " << Hex(uintptr_t(&CONSTRUCTORS_ARRAY_START));
	info() << "Constructors Array End  : " << Hex(uintptr_t(&CONSTRUCTORS_ARRAY_END));

	info() << "Tags Start  : " << Hex(uintptr_t(multiboot_info));
	info() << "Tags Size   : " << Hex(multiboot_info->total_size);

	bootloader_info.tags = multiboot_info;

	multiboot_tag* current_tag = (multiboot_tag*)((char*)multiboot_info + sizeof(multiboot_tag_start));
	while (current_tag->type != MULTIBOOT_TAG_TYPE_END) {
		switch (current_tag->type) {
			case MULTIBOOT_TAG_TYPE_MODULE: {
				multiboot_tag_module* tag = (multiboot_tag_module*)current_tag;
				info() << "Module: " << tag->cmdline;
				info() << "\tStart: " << Hex(tag->mod_start);
				info() << "\tSize: " << Hex(tag->mod_end);
				if (!strcmp(tag->cmdline, "ramdisk")) {
					bootloader_info.ramdisk.start =
					    uintptr_t(Memory::map(tag->mod_start, tag->mod_end - tag->mod_start, PAGE_READWRITE));
					bootloader_info.ramdisk.size = tag->mod_end - tag->mod_start;
				}

				break;
			}
			case MULTIBOOT_TAG_TYPE_MMAP: {
				multiboot_tag_mmap* tag2 = (multiboot_tag_mmap*)current_tag;
				size_t i = 0;
				info() << "Memory Map:";
				while (tag2->entries[i].type != 0) {
					info() << "\tAddr: " << Hex(tag2->entries[i].addr) << " Size: " << Hex(tag2->entries[i].len)
					       << " Type: " << mmap_entry_type_text[tag2->entries[i].type];
					i++;
				}
				break;
			}
		}
		current_tag = reinterpret_cast<multiboot_tag*>(uintptr_t(current_tag) +
		                                               align_to(current_tag->size, MULTIBOOT_INFO_ALIGN));
	}
}
