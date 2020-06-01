

SECTIONS
{
	. = 0xC0100000;
	PROVIDE(KERNEL_START = .);
	.boot	ALIGN(4K) :
	{ 
		*(.boot)
	}
	.bootstrap	: 
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
	PROVIDE(PAGE_TABLES_START = .);
	.stack 	ALIGN(4K) :
	{
		
	}
	PROVIDE(PAGE_TABLES_END = .);
	PROVIDE(KERNEL_STACK_START = .);
	.pages 	ALIGN(4K) :
	{
		*(.pages)
	}
	PROVIDE(KERNEL_STACK_END = .);
	PROVIDE(KERNEL_END = .);

}