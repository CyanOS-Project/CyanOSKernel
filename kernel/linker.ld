

SECTIONS
{
	. = 0x00100000;
	.boot	:  ALIGN(4K) { *(.boot)	}

	. = 0x00100040;
	.bootx	: { *(.bootx)	}

	.text	: ALIGN(4K) {	*(.text)	}
	.data 	: ALIGN(4K) {	*(.data)	}
	.rdata 	: ALIGN(4K) {	*(.rdata)	}
	.bss 	: ALIGN(4K) {	*(.bss)		}

}