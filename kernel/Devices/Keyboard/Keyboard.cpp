#include "Keyboard.h"
#include "Arch/x86/asm.h"
bool Keyboard::pressed_keys[3]{false, false, false};

Keyboard::Keyboard() : DeviceNode{"keyboard", DeviceNode::Type::Keyboard}
{
	PIC::enable_irq(PIC_KEYBOARD);
	ISR::register_isr_handler(keyboard_driver_handler, PIC_KEYBOARD + PIC1_IDT_OFFSET);
}

Keyboard::~Keyboard()
{
}

void Keyboard::keyboard_driver_handler(ISRContextFrame*)
{
	unsigned char data = in8(KBD_DATA_PORT);

	if (data == 0x2A) // SHIFT Pressed
	{
		pressed_keys[0] = 1;
	} else if (data == 0xAA) { // SHIFT Released
		pressed_keys[0] = 0;
	} else if ((data & 0x80) == 0) {
		DebugPort::write(&asccode[data][pressed_keys[0]], 1, DebugColor::Red);
	}
	PIC::acknowledge_pic(PIC_PIT);
}