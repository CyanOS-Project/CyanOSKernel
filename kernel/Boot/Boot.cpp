
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
                .header_addr = VIR_TO_PHY((u32)&my_multiboot2_header),
                .load_addr = unsigned(-1),
                .load_end_addr = 0,
                .bss_end_addr = VIR_TO_PHY(u32(&KERNEL_END))},
    .entry = {.type = MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS,
              .flags = 0,
              .size = sizeof(multiboot_header_tag_entry_address),
              .entry_addr = VIR_TO_PHY((u32)&_kernel_boot_stage1)},
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

extern "C" void kernel_boot_stage2(u32 magic, multiboot_tag_start* boot_info)
{
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
		HLT();
	volatile Module ramdisk_module = get_ramdisk(boot_info);

	Memory::setup();
	Logger::init();

	info() << "Kernel Start: " << Hex(uintptr_t(&KERNEL_START));
	info() << "Kernel End  : " << Hex(uintptr_t(&KERNEL_END));
	info() << "Constructors Array Start: " << Hex(uintptr_t(&CONSTRUCTORS_ARRAY_START));
	info() << "Constructors Array End  : " << Hex(uintptr_t(&CONSTRUCTORS_ARRAY_END));

	if (ramdisk_module.start) {
		info() << "Ramdisk Start  : " << Hex(unsigned(ramdisk_module.start));
		info() << "Ramdisk Size   : " << Hex(ramdisk_module.size);
	} else {
		warn() << "There is no ramdisk!";
	}

	bootloader_info.ramdisk.start = Memory::map(uintptr_t(ramdisk_module.start), ramdisk_module.size, PAGE_READWRITE);
	bootloader_info.ramdisk.size = ramdisk_module.size;

	uintptr_t new_stack = uintptr_t(valloc(0, STACK_SIZE, PAGE_READWRITE));
	SET_STACK(new_stack + STACK_SIZE);
	CALL(kernel_init, &bootloader_info)

	ASSERT_NOT_REACHABLE();
	return;
}

#define PHYSICAL_MEM
Module get_ramdisk(multiboot_tag_start* multiboot_info)
{
	multiboot_tag* current_tag = (multiboot_tag*)((char*)multiboot_info + sizeof(multiboot_tag_start));
	while (current_tag->type != MULTIBOOT_TAG_TYPE_END) {
		switch (current_tag->type) {
			case MULTIBOOT_TAG_TYPE_MODULE: {
				multiboot_tag_module* tag = (multiboot_tag_module*)current_tag;
				if (!strcmp(tag->cmdline, (char*)(VIR_TO_PHY(uintptr_t("ramdisk"))))) {
					return Module{(void*)tag->mod_start, tag->mod_end - tag->mod_start};
				}
				break;
			}
		}
		current_tag = reinterpret_cast<multiboot_tag*>(uintptr_t(current_tag) +
		                                               align_to(current_tag->size, MULTIBOOT_INFO_ALIGN));
	}
	return Module{nullptr, 0};
}

void parse_mbi(multiboot_tag_start* multiboot_info)
{
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
					    Memory::map(uintptr_t(tag->mod_start), tag->mod_end - tag->mod_start, PAGE_READWRITE);
					bootloader_info.ramdisk.size = tag->mod_end - tag->mod_start;
				}

				break;
			}
			case MULTIBOOT_TAG_TYPE_MMAP: {
				multiboot_tag_mmap* tag2 = (multiboot_tag_mmap*)current_tag;
				size_t i = 0;
				info() << "Memory Map:";
				while (tag2->entries[i].type != 0) {
					info() << "\tAddr: " << Hex64(tag2->entries[i].addr) << " Size: " << Hex64(tag2->entries[i].len)
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
