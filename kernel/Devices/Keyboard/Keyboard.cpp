#include "Keyboard.h"
#include "Arch/x86/asm.h"

bool Keyboard::pressed_keys[3] = {0, 0, 0}; // SHIFT-CTRL-ALT

void Keyboard::setup()
{
	PIC::enable_irq(PIC_KEYBOARD);
	ISR::register_isr_handler(keyboard_driver_handler, PIC_KEYBOARD + PIC1_IDT_OFFSET);
}

void Keyboard::keyboard_driver_handler(ISRContextFrame*)
{
	char Data = in8(KBD_DATA_PORT);

	if (Data == 0x2A) // SHIFT Pressed
	{
		pressed_keys[0] = 1;
	} else if (Data == 0xAA) { // SHIFT Released
		pressed_keys[0] = 0;
	} else if ((Data & 0x80) == 0) {
		DebugPort::write(&asccode[Data][pressed_keys[0]], 1, DebugColor::Red);
	}
	PIC::acknowledge_pic(PIC_PIT);
	// outb(0x3D4, 14);// ; 14 tells the framebuffer to expect the highest 8 bits of the position
	// outb(0x3D5, 0x00);// sending the highest 8 bits of 0x0050
	// outb(0x3D4, 15);// 15 tells the framebuffer to expect the lowest 8 bits of the position
	// outb(0x3D5, 0x50);// sending the lowest 8 bits of 0x0050
}