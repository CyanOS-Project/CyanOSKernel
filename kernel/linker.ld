OUTPUT_FORMAT("elf32-i386")
SECTIONS
{
	. = 0xC0100000;
	KERNEL_START = .;
	.multiboot2	ALIGN(4K) : AT(ADDR(.multiboot2) - 0xC0000000)
	{ 
		*(.multiboot2)
	}
	.text	ALIGN(4K) : AT(ADDR(.text) - 0xC0000000)
	{
		*(.bootstrap)	
		*(.text)
	}
	.data	ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
	{	
		*(.data)
	}
	.rdata	ALIGN(4K) : AT(ADDR(.rdata) - 0xC0000000)
	{	
		*(.rdata)

		CONSTRUCTORS_ARRAY_START = .;
		*(.ctors.*)
		*(.ctors)
		CONSTRUCTORS_ARRAY_END = .;
	}
	.bss 	ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000)
	{
		*(.bss)
	}
	. = ALIGN(4K);
	KERNEL_END = .;
}