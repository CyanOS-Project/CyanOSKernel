
#include "apic.h"

bool check_APIC()
{
	uint32_t info = 0;
	asm("MOVL $1,%%EAX;"
	    "CPUID;"
	    "MOVL %%EDX,%0;"
	    : "=r"(info));
	if (!(info & CPUID_FEAT_EDX_APIC))
		PANIC("CPU does not support APIC !");
}