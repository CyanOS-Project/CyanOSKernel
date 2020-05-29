

SECTIONS
{
	. = 0x00100000;
	PROVIDE(KERNEL_PHYSICAL_ADDRESS = .);
	.boot	:  ALIGN(4K) { *(.boot)	}
	.bootx	: { *(.bootstrap)	}
	. = 0xC0100000;
	PROVIDE(KERNEL_VIRTUAL_ADDRESS = .);
	.text	: ALIGN(4K) {	*(.text)	}
	.data 	: ALIGN(4K) {	*(.data)	}
	.rdata 	: ALIGN(4K) {	*(.rdata)	}
	.bss 	: ALIGN(4K) {	*(.bss)		}
	PROVIDE(KERNEL_END = .);

}