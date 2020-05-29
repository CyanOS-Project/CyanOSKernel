
#include "Arch/x86/boot_paging.h"
#include "kernel_init.h"
#include <stdint.h>

#define LOAD_BASE 0x100000
typedef struct __attribute__((__packed__)) {
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

void kernel_boot(void);
__attribute__((section(".boot"))) const volatile multiboot_header my_multiboot_header = { //
    0x1BADB002,                                                                           //
    0x10003,
    (uint32_t) - (0x10003 + 0x1BADB002),
    (uint32_t)&my_multiboot_header,
    LOAD_BASE,
    0,
    0,
    (uint32_t)kernel_boot

};

__attribute__((section(".bootstrap"))) void kernel_boot()
{
	kernel_init();
	asm("hlt");
	return;
}
