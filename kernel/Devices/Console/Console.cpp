#include "Console.h"
#include "Arch/x86/Panic.h"
#include "Kernel_map.h"
#include "Lib/Stdlib.h"
#include "Tasking/ScopedLock.h"
#include "VirtualMemory/Memory.h"

UniquePointer<FSNode> Console::alloc(const StringView& name)
{
	return UniquePointer<FSNode>(new Console(name));
}

Console::Console(const StringView& name) : FSNode(name, 0, 0, NodeType::Root, 0)
{
	initiate_console();
}

Console::~Console()
{
	// FIXME: unmap memory of text VGA.
}

Result<void> Console::open(OpenMode mode, OpenFlags flags)
{
	UNUSED(mode);
	UNUSED(flags);
	return ResultError(ERROR_SUCCESS);
}

Result<void> Console::close(OpenMode mode)
{
	UNUSED(mode);
	return ResultError(ERROR_SUCCESS);
}

Result<void> Console::write(const void* buff, size_t offset, size_t size)
{
	UNUSED(offset);

	const char* _buff = reinterpret_cast<const char*>(buff);
	for (size_t i = 0; i < size; i++) {
		put_char(_buff[i]);
	}
	return ResultError(ERROR_SUCCESS);
}

Result<bool> Console::can_write()
{
	return true;
}

void Console::clearScreen()
{
	for (size_t i = 0; i < VGA_WIDTH; i++) {
		for (size_t i2 = 0; i2 < VGA_HEIGHT; i2++) {
			m_video_ram[i + i2 * VGA_WIDTH] = 0;
		}
	}
	m_hPosition = 0;
	m_vPosition = 0;
}

void Console::initiate_console()
{
	m_hPosition = 0;
	m_vPosition = 0;
	m_video_ram = (uint16_t*)Memory::map(VGATEXTMODE_BUFFER, 0x1000, MEMORY_TYPE::WRITABLE | MEMORY_TYPE::KERNEL);
	if (!m_video_ram)
		PANIC("Cannot map text mode memory");

	clearScreen();
}

void Console::set_mode(TerminalMode Mode)
{
	UNUSED(Mode);
	m_video_ram = (uint16_t*)VGATEXTMODE_BUFFER;
}

uint8_t Console::vga_entry_color(VGAColor fg, VGAColor bg)
{
	return fg | bg << 4;
}

uint16_t Console::vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t)uc | (uint16_t)color << 8;
}

void Console::page_up()
{
	for (size_t i = 0; i < VGA_HEIGHT - 1; i++) {
		memcpy((void*)(m_video_ram + VGA_WIDTH * i), (void*)(m_video_ram + VGA_WIDTH * (i + 1)),
		       VGA_WIDTH * sizeof(short));
	}
	memset((char*)(m_video_ram + VGA_WIDTH * (VGA_HEIGHT - 1)), 0, VGA_WIDTH * sizeof(short));
}

void Console::set_color(VGAColor color, VGAColor backcolor)
{
	m_charColor = vga_entry_color(color, backcolor);
}

void Console::remove_line()
{
	do {
		if (m_hPosition != VGA_WIDTH) // Update Cursor
			m_video_ram[m_hPosition + m_vPosition * VGA_WIDTH] = 0;
		if (m_hPosition > 0) {
			m_hPosition--;
		} else if (m_hPosition == 0 && m_vPosition > 0) {
			m_vPosition--;
			m_hPosition = VGA_WIDTH - 1;
		} else {
			break;
		}
	} while (m_video_ram[m_hPosition + m_vPosition * VGA_WIDTH] == 0 || m_hPosition != 0);
	m_video_ram[m_hPosition + m_vPosition * VGA_WIDTH] = 0;
}

void Console::put_char(const char str)
{
	if (m_vPosition == VGA_HEIGHT) {
		page_up();
		m_vPosition--;
	}
	if (str == '\n') {
		insert_new_line();
	} else if (str == '\b') {
		insert_back_space();
	} else if (str == '\t') {
		for (size_t i = 0; i < 4; i++)
			insert_character(' ');
	} else {
		insert_character(str);
	}
	update_cursor();
}

void Console::update_cursor()
{
	m_video_ram[m_hPosition + m_vPosition * VGA_WIDTH] =
	    vga_entry(0, vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE));
}

void Console::insert_new_line()
{
	if (m_hPosition != VGA_WIDTH) // Update Cursor
		m_video_ram[m_hPosition + m_vPosition * VGA_WIDTH] = 0;
	m_hPosition = 0;
	m_vPosition++;
}

void Console::insert_character(char str)
{
	m_video_ram[m_hPosition + m_vPosition * VGA_WIDTH] = vga_entry(str, m_charColor);
	m_hPosition = (m_hPosition + 1) % VGA_WIDTH;
	m_vPosition = (m_vPosition + (m_hPosition == 0));
}

void Console::insert_back_space()
{
	if (m_hPosition != VGA_WIDTH) // Update Cursor
		m_video_ram[m_hPosition + m_vPosition * VGA_WIDTH] = 0;
	do {
		if (m_hPosition > 0) {
			m_hPosition--;
		} else if (m_hPosition == 0 && m_vPosition > 0) {
			m_vPosition--;
			m_hPosition = VGA_WIDTH - 1;
		} else {
			break;
		}
	} while (m_video_ram[m_hPosition + m_vPosition * VGA_WIDTH] == 0);
	m_video_ram[m_hPosition + m_vPosition * VGA_WIDTH] = 0;
}
