#include "Keyboard.h"
#include "Arch/x86/Asm.h"
#include "Tasking/ScopedLock.h"
#include "Tasking/SpinLock.h"
#include <Algorithms.h>
#include <ErrorCodes.h>

Keyboard* Keyboard::current_instance = nullptr;

UniquePointer<FSNode> Keyboard::alloc(StringView name)
{
	return UniquePointer<FSNode>(new Keyboard(name));
}

Keyboard::Keyboard(StringView name) :
    FSNode{name, 0, 0, NodeType::Device, 1024},
    m_lock{},
    m_wait_queue{},
    m_buffer{1024}
{
	ASSERT(current_instance == nullptr);

	current_instance = this;
	ISR::register_hardware_interrupt_handler(keyboard_driver_handler, PIC_KEYBOARD);
}

Keyboard::~Keyboard() {}

Result<void> Keyboard::open(FileDescription&)
{
	return ResultError(ERROR_SUCCESS);
}

Result<void> Keyboard::close(FileDescription&)
{
	return ResultError(ERROR_SUCCESS);
}

Result<size_t> Keyboard::read(FileDescription& desc, BufferMutableView dest, size_t offset)
{
	UNUSED(offset);

	ScopedLock local_lock(m_lock);

	m_wait_queue.wait_on_event([&]() { return !can_read(desc); }, local_lock);

	size_t size_to_read = min(dest.size(), m_buffer.size());

	for (size_t i = 0; i < size_to_read; i++) {
		dest[i] = m_buffer.dequeue();
	}

	return size_to_read;
}

bool Keyboard::can_read(FileDescription&)
{
	return !m_buffer.is_empty();
}

void Keyboard::enqueue_keystoke(unsigned char data)
{
	ScopedLock local_lock(m_lock);
	if (data == key_pressed(SHIFT_KEY)) { // SHIFT Pressed
		is_shift_pressed = true;
	} else if (data == key_released(SHIFT_KEY)) { // SHIFT Released
		is_shift_pressed = false;
	} else if ((data & 0x80) == 0) {
		if (m_buffer.is_full()) {
			m_buffer.dequeue(); // memory is full get rid of old keyboard strokes.
		}
		m_buffer.queue(asccode[data][static_cast<int>(is_shift_pressed)]);
		m_wait_queue.wake_up_all();
	}
}

void Keyboard::keyboard_driver_handler(ISRContextFrame& frame)
{
	UNUSED(frame);
	ASSERT(current_instance);
	current_instance->enqueue_keystoke(in8(KBD_DATA_PORT));
}

constexpr char Keyboard::key_pressed(char key)
{
	return key;
}

constexpr char Keyboard::key_released(char key)
{
	return key | 0x80;
}