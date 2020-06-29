

SECTIONS
{
	. = 0xC0100000;
	PROVIDE(KERNEL_START = .);
	.boot	ALIGN(4K) :
	{ 
		*(.boot)
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
	.bootstrap	: 
	{ 
		*(.bootstrap)
	}
	PROVIDE(BOOTSTARP_STACK_START = .);
	.stack 	ALIGN(4K) :
	{
		. = . + 4K;
	}
	PROVIDE(BOOTSTARP_STACK_END = .);
	PROVIDE(KERNEL_END = .);
}