#pragma once

#include "Arch/x86/isr.h"
#include "Arch/x86/pic.h"
#include "Devices/DebugPort/DebugPort.h"
#include "Filesystem/FSNode.h"
#include "Tasking/WaitQueue.h"
#include "utils/CircularBuffer.h"
#include "utils/UniquePointer.h"
#include "utils/types.h"

class Keyboard : public FSNode
{
  public:
	static void keyboard_driver_handler(ISRContextFrame*);
	static UniquePointer<FSNode> alloc();

	Keyboard();
	~Keyboard();
	Result<FSNode&> create(const StringView& name, OpenMode mode, OpenFlags flags) override;
	Result<void> open(OpenMode mode, OpenFlags flags) override;
	Result<void> close() override;
	Result<void> read(void* buff, size_t offset, size_t size) override;
	Result<void> write(const void* buff, size_t offset, size_t size) override;
	Result<bool> can_read() override;
	Result<bool> can_write() override;
	Result<void> remove() override;
	Result<void> mkdir(const StringView& name, int flags, int access) override;
	Result<void> link(FSNode& node) override;
	Result<void> unlink(FSNode& node) override;
	Result<FSNode&> dir_lookup(const StringView& file_name) override;

  private:
	Spinlock m_lock;
	WaitQueue m_wait_queue;
	CircularBuffer<char> m_buffer;
	bool pressed_keys[3];
	const char asccode[58][2] = {
	    {0, 0},     {0, 0},       {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'}, {'6', '^'},  {'7', '&'},
	    {'8', '*'}, {'9', '('},   {'0', ')'}, {'-', '_'}, {'=', '+'}, {8, 8},     {9, 9},     {'q', 'Q'},  {'w', 'W'},
	    {'e', 'E'}, {'r', 'R'},   {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'}, {'o', 'O'}, {'p', 'P'},  {'[', '{'},
	    {']', '}'}, {'\n', '\n'}, {0, 0},     {'a', 'A'}, {'s', 'S'}, {'d', 'D'}, {'f', 'F'}, {'g', 'G'},  {'h', 'H'},
	    {'j', 'J'}, {'k', 'K'},   {'l', 'L'}, {';', ':'}, {39, 34},   {'`', '~'}, {0, 0},     {'\\', '|'}, {'z', 'Z'},
	    {'x', 'X'}, {'c', 'C'},   {'v', 'V'}, {'b', 'B'}, {'n', 'N'}, {'m', 'M'}, {',', '<'}, {'.', '>'},  {'/', '?'},
	    {0, 0},     {0, 0},       {0, 0},     {' ', ' '},
	};
	void enqueue_keystoke(unsigned char data);

	static Keyboard* current_instance;
	static const int KBD_DATA_PORT = 0x60;
};
