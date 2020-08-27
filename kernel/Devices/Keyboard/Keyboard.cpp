#include "Keyboard.h"
#include "Arch/x86/Asm.h"
#include "Tasking/ScopedLock.h"
#include "Tasking/SpinLock.h"
#include "Utils/ErrorCodes.h"

Keyboard* Keyboard::current_instance = nullptr;

UniquePointer<FSNode> Keyboard::alloc(const StringView& name)
{
	return UniquePointer<FSNode>(new Keyboard(name));
}

Keyboard::Keyboard(const StringView& name) :
    FSNode{name, 0, 0, NodeType::Device, 1024},
    m_lock{},
    m_wait_queue{},
    m_buffer{1024},
    pressed_keys{false, false, false}
{
	ASSERT(current_instance == nullptr);

	current_instance = this;
	m_lock.init();
	PIC::enable_irq(PIC_KEYBOARD);
	ISR::register_isr_handler(keyboard_driver_handler, PIC_KEYBOARD + PIC1_IDT_OFFSET);
}

Keyboard::~Keyboard()
{
}

Result<void> Keyboard::open(FileDescription&)
{
	return ResultError(ERROR_SUCCESS);
}

Result<void> Keyboard::close(FileDescription&)
{
	return ResultError(ERROR_SUCCESS);
}

Result<void> Keyboard::read(FileDescription&, void* buff, size_t offset, size_t size)
{
	UNUSED(offset);

	ScopedLock local_lock(m_lock);
	while (m_buffer.size() < size) {
		local_lock.release();
		Thread::current->wait_on(m_wait_queue);
		local_lock.acquire();
	}

	char* _buf = static_cast<char*>(buff);
	for (size_t i = 0; i < size; i++) {
		_buf[i] = m_buffer.dequeue();
	}

	return ResultError(ERROR_SUCCESS);
}

Result<bool> Keyboard::can_read(FileDescription&)
{
	ScopedLock local_lock(m_lock);
	return m_buffer.is_empty();
}

void Keyboard::enqueue_keystoke(unsigned char data)
{
	ScopedLock local_lock(m_lock);
	if (data == 0x2A) // SHIFT Pressed
	{
		pressed_keys[0] = 1;
	} else if (data == 0xAA) { // SHIFT Released
		pressed_keys[0] = 0;
	} else if ((data & 0x80) == 0) {
		if (m_buffer.is_full()) {
			m_buffer.dequeue(); // memory is full get rid of old keyboard strokes.
		}
		m_buffer.queue(asccode[data][pressed_keys[0]]);
		m_wait_queue.wake_up_all();
	}
}

void Keyboard::keyboard_driver_handler(ISRContextFrame* frame)
{
	UNUSED(frame);
	current_instance->enqueue_keystoke(in8(KBD_DATA_PORT));
	PIC::acknowledge_pic(PIC_PIT);
}