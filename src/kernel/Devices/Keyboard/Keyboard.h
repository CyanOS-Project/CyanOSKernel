#pragma once

#include "Arch/x86/Isr.h"
#include "Arch/x86/Pic.h"
#include "Devices/DebugPort/DebugPort.h"
#include "Filesystem/FSNode.h"
#include "Tasking/WaitQueue.h"
#include <CircularBuffer.h>
#include <Types.h>
#include <UniquePointer.h>

class Keyboard : public FSNode
{
  public:
	static void keyboard_driver_handler(ISRContextFrame&);
	static UniquePointer<FSNode> alloc(StringView name);

	~Keyboard();
	Result<void> open(FileDescription&) override;
	Result<void> close(FileDescription&) override;
	Result<size_t> read(FileDescription&, BufferMutableView dest, size_t offset) override;
	bool can_read(FileDescription&) override;

  private:
	Spinlock m_lock;
	WaitQueue m_wait_queue;
	CircularBuffer<char> m_buffer;
	bool is_shift_pressed = false;

	static constexpr char asccode[58][2] = {
	    {0, 0},     {0, 0},       {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'}, {'6', '^'},  {'7', '&'},
	    {'8', '*'}, {'9', '('},   {'0', ')'}, {'-', '_'}, {'=', '+'}, {8, 8},     {9, 9},     {'q', 'Q'},  {'w', 'W'},
	    {'e', 'E'}, {'r', 'R'},   {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'}, {'o', 'O'}, {'p', 'P'},  {'[', '{'},
	    {']', '}'}, {'\n', '\n'}, {0, 0},     {'a', 'A'}, {'s', 'S'}, {'d', 'D'}, {'f', 'F'}, {'g', 'G'},  {'h', 'H'},
	    {'j', 'J'}, {'k', 'K'},   {'l', 'L'}, {';', ':'}, {39, 34},   {'`', '~'}, {0, 0},     {'\\', '|'}, {'z', 'Z'},
	    {'x', 'X'}, {'c', 'C'},   {'v', 'V'}, {'b', 'B'}, {'n', 'N'}, {'m', 'M'}, {',', '<'}, {'.', '>'},  {'/', '?'},
	    {0, 0},     {0, 0},       {0, 0},     {' ', ' '},
	};

	explicit Keyboard(StringView name);
	void enqueue_keystoke(unsigned char data);
	constexpr char key_pressed(char key);
	constexpr char key_released(char key);

	static Keyboard* current_instance;
	static constexpr int KBD_DATA_PORT = 0x60;
	static constexpr char SHIFT_KEY = 0x2A;
};
