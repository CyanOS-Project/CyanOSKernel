#include "console.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
volatile uint16_t* video_ram = 0;
volatile int vPosition = 0, hPosition = 0;
uint8_t charColor = 0x0F;
SpinLock printf_lock;

void clearScreen()
{
	for (size_t i = 0; i < VGA_WIDTH; i++) {
		for (size_t i2 = 0; i2 < VGA_HEIGHT; i2++) {
			video_ram[i + i2 * VGA_WIDTH] = 0;
		}
	}
	hPosition = 0;
	vPosition = 0;
}

void initiate_console()
{
	spinlock_init(&printf_lock);
	hPosition = 0;
	vPosition = 0;
	Memory::map((void*)(KERNEL_VIRTUAL_ADDRESS + VGATEXTMODE_BUFFER), (void*)VGATEXTMODE_BUFFER, 0x1000,
	            MEMORY_TYPE::WRITABLE);
	video_ram = (uint16_t*)(KERNEL_VIRTUAL_ADDRESS + VGATEXTMODE_BUFFER);
	clearScreen();
}

void printf(const char* s, ...)
{
	spinlock_acquire(&printf_lock);
	unsigned char c;
	va_list ap;
	va_start(ap, s);
	while ((c = *s++)) {
		if (c == 0)
			break;
		else if (c == '%') {
			formatEscapeCharacters(c, s, va_arg(ap, int));
			s++;
		} else
			putChar(c);
	}
	spinlock_release(&printf_lock);
}

void displayMemory(const char* Address, int Size)
{
	spinlock_acquire(&printf_lock);
	for (size_t i = 0; i < Size; i++) {
		if ((i % 8 == 0))
			__printf("\n");
		else if (i % 4 == 0)
			__printf("          ");
		__printf("%X ", Address[i]);
	}
	__printf("\n");
	spinlock_release(&printf_lock);
}

void printStatus(const char* msg, bool Success)
{
	spinlock_acquire(&printf_lock);
	__printf("[");
	if (Success) {
		setColor(VGAColor::VGA_COLOR_GREEN, VGAColor::VGA_COLOR_BLACK);
		__printf("  OK  ");
	} else {
		setColor(VGAColor::VGA_COLOR_RED, VGAColor::VGA_COLOR_BLACK);
		__printf(" FAIL ");
	}
	setColor(VGAColor::VGA_COLOR_WHITE, VGAColor::VGA_COLOR_BLACK);
	__printf("] %s.\n", msg);
	spinlock_release(&printf_lock);
}

void __printf(const char* s, ...)
{
	unsigned char c;
	va_list ap;
	va_start(ap, s);
	while ((c = *s++)) {
		if (c == 0)
			break;
		else if (c == '%') {
			formatEscapeCharacters(c, s, va_arg(ap, int));
			s++;
		} else
			putChar(c);
	}
}

void setMode(TerminalMode Mode)
{
	video_ram = (uint16_t*)VGATEXTMODE_BUFFER;
}

inline uint8_t vga_entry_color(VGAColor fg, VGAColor bg)
{
	return fg | bg << 4;
}

inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t)uc | (uint16_t)color << 8;
}

void pageUp()
{
	for (size_t i = 0; i < VGA_HEIGHT - 1; i++) {
		memcpy((void*)(video_ram + VGA_WIDTH * i), (void*)(video_ram + VGA_WIDTH * (i + 1)), VGA_WIDTH * sizeof(short));
	}
	memset((char*)(video_ram + VGA_WIDTH * (VGA_HEIGHT - 1)), 0, VGA_WIDTH * sizeof(short));
}

void setColor(VGAColor color, VGAColor backcolor)
{
	charColor = vga_entry_color(color, backcolor);
}

void removeLine()
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

void putChar(char str)
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

void updateCursor()
{
	video_ram[hPosition + vPosition * VGA_WIDTH] = vga_entry(0, vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE));
}

void insertNewLine()
{
	if (hPosition != VGA_WIDTH) // Update Cursor
		video_ram[hPosition + vPosition * VGA_WIDTH] = 0;
	hPosition = 0;
	vPosition++;
}

void insertCharacter(char str)
{
	video_ram[hPosition + vPosition * VGA_WIDTH] = vga_entry(str, charColor);
	hPosition = (hPosition + 1) % VGA_WIDTH;
	vPosition = (vPosition + (hPosition == 0));
}

void insertBackSpace()
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

void formatEscapeCharacters(unsigned char c, const char* s, int cur_arg)
{
	int size = 0;
	c = *s++;
	if (c >= '0' && c <= '9') {
		size = c - '0';
		c = *s++;
	}
	if (c == 'd') {
		formatDecimal(cur_arg, size);
	} else if (c == 'u') {
		formatUnsigned(cur_arg, size);
	} else if (c == 'x' || c == 'X') {
		formatHex(cur_arg, size, c);
	} else if (c == 'p') {
		formatPointer(cur_arg, size);
	} else if (c == 's') {
		formatString((char*)cur_arg);
	}
}

void formatString(char* arg)
{
	__printf(arg);
}

void formatPointer(int arg, int size)
{
	char buf[16];
	itoa(buf, arg, 16);
	size = 8;

	int buflen = strlen(buf);
	int i, j;
	if (buflen < size)
		for (i = size, j = buflen; i >= 0; i--, j--)
			buf[i] = (j >= 0) ? buf[j] : '0';
	__printf("0x%s", buf);
}

void formatHex(int arg, int size, unsigned char c)
{
	char buf[16];
	itoa(buf, arg, 16);

	int buflen = strlen(buf);
	int i, j;
	if (buflen < size)
		for (i = size, j = buflen; i >= 0; i--, j--)
			buf[i] = (j >= 0) ? buf[j] : '0';
	if (c == 'X')
		toupper(buf);
	__printf("0x%s", buf);
}

void formatUnsigned(int arg, int size)
{
	char buf[16];
	itoa(buf, arg, 10);

	int buflen = strlen(buf);
	int i, j;
	if (buflen < size)
		for (i = size, j = buflen; i >= 0; i--, j--)
			buf[i] = (j >= 0) ? buf[j] : '0';
	__printf(buf);
}

void formatDecimal(int arg, int size)
{
	char buf[16];
	int neg = 0;
	if (arg < 0) {
		arg = 0 - arg;
		neg++;
	} else
		arg = arg;
	itoa(buf, arg, 10);
	int buflen = strlen(buf);
	int i, j;
	if (buflen < size)
		for (i = size, j = buflen; i >= 0; i--, j--)
			buf[i] = (j >= 0) ? buf[j] : '0';
	if (neg)
		__printf("-%s", buf);
	else
		__printf(buf);
}
