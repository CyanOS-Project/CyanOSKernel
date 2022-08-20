#include "Pic.h"

void PIC::setup()
{
	// ICW1
	out8(PIC1_COMMAND, ICW1_ICW4 | ICW1_INIT);
	out8(PIC2_COMMAND, ICW1_ICW4 | ICW1_INIT);
	// ICW2
	out8(PIC1_DATA, PIC1_IDT_OFFSET);
	out8(PIC2_DATA, PIC2_IDT_OFFSET);
	// ICW3
	out8(PIC1_DATA, 0x4);
	out8(PIC2_DATA, 0x2);
	// ICW4
	out8(PIC1_DATA, ICW4_BUF_MASTER);
	out8(PIC2_DATA, ICW4_BUF_SLAVE);
	// Mask
	out8(PIC1_DATA, 0xFB); //	Activates IR 2 only.
	out8(PIC2_DATA, 0xFF); //	Disable all IRs.
}

void PIC::enable_irq(uint8_t interrupt_line)
{
	uint8_t pic_mask;
	if (interrupt_line < 8) { // PIC1
		pic_mask = in8(PIC1_DATA);
		out8(PIC1_DATA, pic_mask & ~(1 << interrupt_line));
	} else { // PIC2
		pic_mask = in8(PIC2_DATA);
		out8(PIC2_DATA, pic_mask & ~(1 << (interrupt_line - 8)));
	}
}

void PIC::acknowledge_pic(uint8_t irq)
{
	if (irq < (PIC1_IDT_OFFSET + 8)) { // PIC1
		out8(PIC1_COMMAND, PIC_EOI);
	} else { // PIC2
		out8(PIC1_COMMAND, PIC_EOI);
		out8(PIC2_COMMAND, PIC_EOI);
	}
}