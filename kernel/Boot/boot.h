#pragma once

#include "kernel_init.h"
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
	multiboot_header header __attribute__((aligned(MULTIBOOT_HEADER_ALIGN)));
	multiboot_header_tag_address address __attribute__((aligned(MULTIBOOT_HEADER_ALIGN)));
	multiboot_header_tag_entry_address entry __attribute__((aligned(MULTIBOOT_HEADER_ALIGN)));
	multiboot_header_tag end __attribute__((aligned(MULTIBOOT_HEADER_ALIGN)));
} Mutiboot2_Header;
#define MULTIBOOT2_HEADER_CHECKSUM ((uint32_t)(-sizeof(Mutiboot2_Header) - MULTIBOOT2_HEADER_MAGIC))

void kernel_boot();
void parse_mbi(uintptr_t multiboot_info, BootloaderInfo& info);