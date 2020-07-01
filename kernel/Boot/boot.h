#pragma once

#include "multiboot2.h"
#include "utils/types.h"

/*typedef struct __attribute__((__packed__)) {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
    uint32_t mode_type;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
} multiboot_header;

typedef struct __attribute__((__packed__)) {
    uint16_t tag;
    uint16_t flags;
    uint32_t size;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
} multiboot2_address_tag;

typedef struct __attribute__((__packed__)) {
    uint16_t tag;
    uint16_t flags;
    uint32_t size;
    uint32_t entry_addr;
} multiboot2_entry_address_tag;

typedef struct __attribute__((__packed__)) {
    uint16_t tag;
    uint16_t flags;
    uint32_t size;
} multiboot2_general_tag;*/

typedef struct __attribute__((__packed__)) {
	multiboot_header header __attribute__((aligned(8)));
	multiboot_header_tag_address address __attribute__((aligned(8)));
	multiboot_header_tag_entry_address entry __attribute__((aligned(8)));
	multiboot_header_tag end __attribute__((aligned(8)));
} Mutiboot2_Header;
#define MULTIBOOT2_HEADER_CHECKSUM ((uint32_t)(-sizeof(Mutiboot2_Header) - MULTIBOOT2_HEADER_MAGIC))

struct MultibootInfo {
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
	uint32_t mmap_length;
	uint32_t mmap_addr;
	uint32_t drives_length;
	uint32_t drives_addr;
	uint32_t config_table;
	uint32_t boot_loader_name;
	uint32_t apm_table;
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint32_t vbe_mode;
	uint32_t vbe_interface_seg;
	uint32_t vbe_interface_off;
	uint32_t vbe_interface_len;
} __attribute__((packed));

void kernel_boot();