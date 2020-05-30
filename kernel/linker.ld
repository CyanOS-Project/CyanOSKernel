

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
	}
	.rdata 	ALIGN(4K) : 
	{
		*(.rdata)
	}
	.bss 	ALIGN(4K) :
	{
		*(.bss)
	}
	. = . + 4K;
	PROVIDE(KERNEL_STACK = .);
	PROVIDE(KERNEL_END = .);

}