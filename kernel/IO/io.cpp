#include "io.h"
#include "Arch/x86/apic.h"
#include "VirtualMemory/memory.h"

void setup_io_devices()
{
	check_APIC();
}