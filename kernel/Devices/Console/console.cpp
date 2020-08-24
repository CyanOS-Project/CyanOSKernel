#include "console.h"
#include "Arch/x86/panic.h"
#include "Lib/stdlib.h"
#include "Tasking/ScopedLock.h"
#include "VirtualMemory/memory.h"
#include "kernel_map.h"
#include <stdarg.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
volatile uint16_t* video_ram = 0;
volatile int vPosition = 0, hPosition = 0;
uint8_t charColor = 0x0F;
Spinlock lock;

UniquePointer<FSNode> Console::alloc()
{
	return UniquePointer<FSNode>(new Console);
}

Console::Console() : FSNode("console", 0, 0, NodeType::Root, 0)
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

Result<void> Console::close()
{
	return ResultError(ERROR_SUCCESS);
}

Result<void> Console::write(const void* buff, size_t offset, size_t size)
{
	UNUSED(offset);

	const char* _buff = reinterpret_cast<const char*>(buff);
	for (size_t i = 0; i < size; i++) {
		putChar(_buff[i]);
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
			video_ram[i + i2 * VGA_WIDTH] = 0;
		}
	}
	hPosition = 0;
	vPosition = 0;
}

void Console::initiate_console()
{
	lock.init();
	hPosition = 0;
	vPosition = 0;
	video_ram = (uint16_t*)Memory::map(VGATEXTMODE_BUFFER, 0x1000, MEMORY_TYPE::WRITABLE | MEMORY_TYPE::KERNEL);
	if (!video_ram)
		PANIC("Cannot map text mode memory");

	clearScreen();
}

void Console::setMode(TerminalMode Mode)
{
	UNUSED(Mode);
	video_ram = (uint16_t*)VGATEXTMODE_BUFFER;
}

uint8_t Console::vga_entry_color(VGAColor fg, VGAColor bg)
{
	return fg | bg << 4;
}

uint16_t Console::vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t)uc | (uint16_t)color << 8;
}

void Console::pageUp()
{
	for (size_t i = 0; i < VGA_HEIGHT - 1; i++) {
		memcpy((void*)(video_ram + VGA_WIDTH * i), (void*)(video_ram + VGA_WIDTH * (i + 1)), VGA_WIDTH * sizeof(short));
	}
	memset((char*)(video_ram + VGA_WIDTH * (VGA_HEIGHT - 1)), 0, VGA_WIDTH * sizeof(short));
}

void Console::setColor(VGAColor color, VGAColor backcolor)
{
	charColor = vga_entry_color(color, backcolor);
}

void Console::removeLine()
{
	do {
		if (hPosition != VGA_WIDTH) // Update Cursor
			video_ram[hPosition + vPosition * VGA_WIDTH] = 0;
		if (hPosition > 0) {
			hPosition--;
		} else if (hPosition == 0 && vPosition > 0) {
			vPosition--;
			hPosition = VGA_WIDTH - 1;
		} else {
			break;
		}
	} while (video_ram[hPosition + vPosition * VGA_WIDTH] == 0 || hPosition != 0);
	video_ram[hPosition + vPosition * VGA_WIDTH] = 0;
}

void Console::putChar(const char str)
{
	if (vPosition == VGA_HEIGHT) {
		pageUp();
		vPosition--;
	}
	if (str == '\n') {
		insertNewLine();
	} else if (str == '\b') {
		insertBackSpace();
	} else if (str == '\t') {
		for (size_t i = 0; i < 4; i++)
			insertCharacter(' ');
	} else {
		insertCharacter(str);
	}
	updateCursor();
}

void Console::updateCursor()
{
	video_ram[hPosition + vPosition * VGA_WIDTH] = vga_entry(0, vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE));
}

void Console::insertNewLine()
{
	if (hPosition != VGA_WIDTH) // Update Cursor
		video_ram[hPosition + vPosition * VGA_WIDTH] = 0;
	hPosition = 0;
	vPosition++;
}

void Console::insertCharacter(char str)
{
	video_ram[hPosition + vPosition * VGA_WIDTH] = vga_entry(str, charColor);
	hPosition = (hPosition + 1) % VGA_WIDTH;
	vPosition = (vPosition + (hPosition == 0));
}

void Console::insertBackSpace()
{
	if (hPosition != VGA_WIDTH) // Update Cursor
		video_ram[hPosition + vPosition * VGA_WIDTH] = 0;
	do {
		if (hPosition > 0) {
			hPosition--;
		} else if (hPosition == 0 && vPosition > 0) {
			vPosition--;
			hPosition = VGA_WIDTH - 1;
		} else {
			break;
		}
	} while (video_ram[hPosition + vPosition * VGA_WIDTH] == 0);
	video_ram[hPosition + vPosition * VGA_WIDTH] = 0;
}
