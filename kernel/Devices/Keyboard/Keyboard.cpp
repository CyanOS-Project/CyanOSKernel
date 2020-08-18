#include "Keyboard.h"
#include "Arch/x86/asm.h"
#include "utils/ErrorCodes.h"

bool Keyboard::pressed_keys[3]{false, false, false};
CircularBuffer<char>* Keyboard::m_buffer;

Keyboard::Keyboard() : DeviceNode{"keyboard", DeviceNode::Type::Keyboard}
{
	m_buffer = new CircularBuffer<char>{1024};
	PIC::enable_irq(PIC_KEYBOARD);
	ISR::register_isr_handler(keyboard_driver_handler, PIC_KEYBOARD + PIC1_IDT_OFFSET);
}

Keyboard::~Keyboard()
{
	delete m_buffer;
}

Result<void> Keyboard::receive(void* buffer, size_t count)
{
	if (m_buffer->available_size() < count) {
		return ResultError(ERROR_EOF);
	}

	char* _buf = static_cast<char*>(buffer);
	for (size_t i = 0; i < count; i++) {
		_buf[i] = m_buffer->dequeue();
	}

	return ResultError(ERROR_SUCCESS);
}

Result<void> Keyboard::send(void* buffer, size_t count)
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> Keyboard::can_read()
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> Keyboard::can_write()
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> Keyboard::control()
{
	return ResultError(ERROR_INVALID_OPERATION);
}

void Keyboard::keyboard_driver_handler(ISRContextFrame* frame)
{
	unsigned char data = in8(KBD_DATA_PORT);

	if (data == 0x2A) // SHIFT Pressed
	{
		pressed_keys[0] = 1;
	} else if (data == 0xAA) { // SHIFT Released
		pressed_keys[0] = 0;
	} else if ((data & 0x80) == 0) {
		if (m_buffer->is_full()) {
			m_buffer->dequeue(); // memory is full get rid of old keyboard strokes.
		}
		m_buffer->queue(data);
	}
	PIC::acknowledge_pic(PIC_PIT);
}