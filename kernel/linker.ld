

SECTIONS
{
	. = 0x00100000;
	.boot	:  { *(.boot)	}

	. = 0x00100040;
	.bootx	: { *(.bootx)	}

	.text	: {	*(.text)	}
	.data 	: {	*(.data)	}
	.rdata 	: {	*(.rdata)	}
	.bss 	: {	*(.bss)		}

	.debug_info 	: { *(.debug_info)		}
	.debug_ranges	: { *(.debug_ranges)	}
	.debug_aranges	: { *(.debug_aranges)	}
	.debug_abbrev	: { *(.debug_abbrev)	}
	.debug_line		: { *(.debug_line)		}
	.debug_str		: { *(.debug_str)		}
	.eh_frame		: { *(.eh_frame)		}

	.ext : { *(.*)	}
	

}