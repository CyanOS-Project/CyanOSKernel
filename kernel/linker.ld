OUTPUT_FORMAT("elf32-i386")

SECTIONS
{
	. = 0xC0100000;
	_KERNEL_START = .;
	.multiboot2	ALIGN(4K) :
	{ 
		*(.multiboot2)
	}
	.bootstrap	ALIGN(4K): 
	{ 
		*(.bootstrap)	
	}
	.text	ALIGN(4K) :
	{	
		*(.text)
	}
	.data	ALIGN(4K)	:
	{	
		*(.data)
		*(.rdata)
	}
	.bss 	ALIGN(4K) :
	{
		*(.bss)
	}
	_KERNEL_END = .;
}