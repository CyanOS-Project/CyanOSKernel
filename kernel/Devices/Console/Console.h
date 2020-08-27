#pragma once
#include "Filesystem/FSNode.h"
#include "Tasking/SpinLock.h"
#include "Utils/Types.h"
#include "Utils/UniquePointer.h"

#define VGATEXTMODE_BUFFER 0x000B8000

typedef enum { MODE_80H25V = 0 } TerminalMode;

typedef enum {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
} VGAColor;

class Console : public FSNode
{
  public:
	static UniquePointer<FSNode> alloc(const StringView& name);

	~Console();
	Result<void> open(FileDescription&) override;
	Result<void> close(FileDescription&) override;
	Result<void> write(FileDescription&, const void* buff, size_t offset, size_t size) override;
	Result<bool> can_write(FileDescription&) override;

  private:
	Spinlock m_lock;
	volatile uint16_t* m_video_ram = 0;
	int m_vPosition = 0, m_hPosition = 0;
	uint8_t m_charColor = 0x0F;
	static const size_t VGA_WIDTH = 80;
	static const size_t VGA_HEIGHT = 25;

	explicit Console(const StringView& name);
	void set_mode(TerminalMode Mode);
	void set_color(VGAColor color, VGAColor backcolor);
	void put_char(const char str);
	void clearScreen();
	void initiate_console();
	void remove_line();
	void page_up();
	void update_cursor();
	void insert_new_line();
	void insert_character(char str);
	void insert_back_space();
	uint8_t vga_entry_color(VGAColor fg, VGAColor bg);
	uint16_t vga_entry(unsigned char uc, uint8_t color);
};
