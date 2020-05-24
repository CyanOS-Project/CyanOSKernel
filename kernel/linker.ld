

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

	.debug_info 	: ALIGN(4K) { *(.debug_info)		}
	.debug_ranges	: ALIGN(4K) { *(.debug_ranges)	}
	.debug_aranges	: ALIGN(4K) { *(.debug_aranges)	}
	.debug_abbrev	: ALIGN(4K) { *(.debug_abbrev)	}
	.debug_line		: ALIGN(4K) { *(.debug_line)		}
	.debug_str		: ALIGN(4K) { *(.debug_str)		}
	.eh_frame		: ALIGN(4K) { *(.eh_frame)		}

	.ext : ALIGN(4K) { *(.*)	}
	

}