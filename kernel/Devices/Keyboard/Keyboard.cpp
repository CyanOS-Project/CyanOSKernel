#include "Keyboard.h"
#include "Arch/x86/asm.h"
#include "Tasking/ScopedLock.h"
#include "utils/ErrorCodes.h"

Keyboard* Keyboard::current_instance = nullptr;

UniquePointer<FSNode> Keyboard::alloc()
{
	return UniquePointer<FSNode>(new Keyboard);
}

Keyboard::Keyboard() :
    FSNode{"keyboard", 0, 0, NodeType::Device, 1024},
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

Result<FSNode&> Keyboard::create(const StringView& name, OpenMode mode, OpenFlags flags)
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> Keyboard::open(OpenMode mode, OpenFlags flags)
{
	UNUSED(mode);
	UNUSED(flags);

	return ResultError(ERROR_SUCCESS);
}

Result<void> Keyboard::close()
{
	return ResultError(ERROR_SUCCESS);
}

Result<void> Keyboard::read(void* buff, size_t offset, size_t size)
{
	ScopedLock local_lock(m_lock);
	if (m_buffer.size() < size) {
		local_lock.release();
		Thread::current->wait_on(m_wait_queue);
		local_lock.acquire();
	}

	char* _buf = static_cast<char*>(buff);
	for (size_t i = 0; i < size; i++) {
		_buf[i] = m_buffer.dequeue();
	}
	m_wait_queue.wake_up_all();

	return ResultError(ERROR_SUCCESS);
}

Result<void> Keyboard::write(const void* buff, size_t offset, size_t size)
{
	UNUSED(buff);
	UNUSED(size);
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<bool> Keyboard::can_read()
{
	ScopedLock local_lock(m_lock);
	return m_buffer.is_empty();
}

Result<bool> Keyboard::can_write()
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> Keyboard::remove()
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> Keyboard::mkdir(const StringView& name, int flags, int access)
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> Keyboard::link(FSNode& node)
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<void> Keyboard::unlink(FSNode& node)
{
	return ResultError(ERROR_INVALID_OPERATION);
}

Result<FSNode&> Keyboard::dir_lookup(const StringView& file_name)
{
	return ResultError(ERROR_INVALID_OPERATION);
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