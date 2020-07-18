

SECTIONS
{
	. = 0xC0100000;
	PROVIDE(KERNEL_START = .);
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
	PROVIDE(KERNEL_END = .);
}