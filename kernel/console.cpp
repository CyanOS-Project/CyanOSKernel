#include "console.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
volatile uint16_t* video_ram = (uint16_t*)VGATEXTMODE_BUFFER;
volatile int vPosition = 0, hPosition = 0;
uint8_t charColor = 0x0F;

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
		memcpy((char*)(video_ram + VGA_WIDTH * i), (char*)(video_ram + VGA_WIDTH * (i + 1)), VGA_WIDTH * sizeof(short));
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
	clearScreen();
}
void printf(const char* s, ...)
{
	va_list ap;
	unsigned char c;
	va_start(ap, s);
	while ((c = *s++)) {
		if (c == 0)
			break;
		else if (c == '%') {
			formatEscapeCharacters(c, s, ap);
			s++;
		} else
			putChar(c);
	}
	return;
}
void formatEscapeCharacters(unsigned char c, const char* s, va_list ap)
{
	int size = 0;
	c = *s++;
	if (c >= '0' && c <= '9') {
		size = c - '0';
		c = *s++;
	}
	if (c == 'd') {
		formatDecimal(ap, size);
	} else if (c == 'u') {
		formatUnsigned(ap, size);
	} else if (c == 'x' || c == 'X') {
		formatHex(ap, size, c);
	} else if (c == 'p') {
		formatPointer(ap, size);
	} else if (c == 's') {
		formatString(ap);
	}
}
void formatString(va_list ap)
{
	printf((char*)va_arg(ap, int));
}
void formatPointer(va_list ap, int size)
{
	char buf[16];
	unsigned int uival;
	uival = va_arg(ap, int);
	itoa(buf, uival, 16);
	size = 8;

	int buflen = strlen(buf);
	int i, j;
	if (buflen < size)
		for (i = size, j = buflen; i >= 0; i--, j--)
			buf[i] = (j >= 0) ? buf[j] : '0';
	printf("0x%s", buf);
}
void formatHex(va_list ap, int size, unsigned char c)
{
	char buf[16];
	unsigned int uival;
	uival = va_arg(ap, int);
	itoa(buf, uival, 16);

	int buflen = strlen(buf);
	int i, j;
	if (buflen < size)
		for (i = size, j = buflen; i >= 0; i--, j--)
			buf[i] = (j >= 0) ? buf[j] : '0';
	if (c == 'X')
		toupper(buf);
	printf("0x%s", buf);
}
void formatUnsigned(va_list ap, int size)
{
	char buf[16];
	unsigned int uival;
	uival = va_arg(ap, int);
	itoa(buf, uival, 10);

	int buflen = strlen(buf);
	int i, j;
	if (buflen < size)
		for (i = size, j = buflen; i >= 0; i--, j--)
			buf[i] = (j >= 0) ? buf[j] : '0';
	printf(buf);
}
void formatDecimal(va_list ap, int size)
{
	char buf[16];
	int neg = 0;
	unsigned int uival;
	int ival;
	ival = va_arg(ap, int);
	if (ival < 0) {
		uival = 0 - ival;
		neg++;
	} else
		uival = ival;
	itoa(buf, uival, 10);
	int buflen = strlen(buf);
	int i, j;
	if (buflen < size)
		for (i = size, j = buflen; i >= 0; i--, j--)
			buf[i] = (j >= 0) ? buf[j] : '0';
	if (neg)
		printf("-%s", buf);
	else
		printf(buf);
}
void displayMemory(char* Address, int Size)
{
	for (size_t i = 0; i < Size; i++) {
		if ((i % 8 == 0))
			printf("\n");
		else if (i % 4 == 0)
			printf("          ");
		printf("%X ", Address[i]);
	}
	printf("\n");
}
void printSuccess(char* msg, bool Success)
{
	printf("[");
	if (Success) {
		setColor(VGAColor::VGA_COLOR_GREEN, VGAColor::VGA_COLOR_BLACK);
		printf(" OK ");
	} else {
		setColor(VGAColor::VGA_COLOR_RED, VGAColor::VGA_COLOR_BLACK);
		printf(" FAIL ");
	}
	setColor(VGAColor::VGA_COLOR_WHITE, VGAColor::VGA_COLOR_BLACK);
	printf("] %s.\n", msg);
}