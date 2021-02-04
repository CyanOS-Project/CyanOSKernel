#include "Intel_i217.h"
#include "Arch/x86/Asm.h"
#include "Devices/DebugPort/Logger.h"
#include "VirtualMemory/Memory.h"

static uint8_t* ports = nullptr;
void writeCommand(uint16_t p_address, uint32_t p_value)
{
	*((uint32_t*)(ports + p_address)) = p_value;
}
uint32_t readCommand(uint16_t p_address)
{
	return *((uint32_t*)(ports + p_address));
}

bool detectEEProm()
{
	bool eerprom_exists = false;
	uint32_t val = 0;
	writeCommand(REG_EEPROM, 0x1);
	return readCommand(REG_EEPROM) & 0x10;
}

uint32_t eepromRead(uint8_t addr)
{
	uint16_t data = 0;
	uint32_t tmp = 0;

	writeCommand(REG_EEPROM, (1) | ((uint32_t)(addr) << 2));
	while (!((tmp = readCommand(REG_EEPROM)) & (1 << 1)))
		;
	data = (uint16_t)((tmp >> 16) & 0xFFFF);
	return data;
}

void test_ethernet(const GenericPCIDevice& device)
{
	ports = (uint8_t*)Memory::map(0xFEB80000, 0x6000, PAGE_READWRITE);

	if (detectEEProm()) {
		info() << "There is eeprom";
	} else {
		info() << "There is eeprom";
	}

	info() << readCommand(0x5400);
	uint8_t mac[6];
	uint32_t temp;
	temp = eepromRead(0);
	mac[0] = temp & 0xff;
	mac[1] = temp >> 8;
	temp = eepromRead(1);
	mac[2] = temp & 0xff;
	mac[3] = temp >> 8;
	temp = eepromRead(2);
	mac[4] = temp & 0xff;
	mac[5] = temp >> 8;

	for (size_t i = 0; i < 6; i++) {
		info() << Hex(mac[i]);
	}
}