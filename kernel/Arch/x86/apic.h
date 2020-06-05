#include "panic.h"
#include "types.h"

#define CPUID_FEAT_EDX_APIC (1 << 9)

bool check_APIC();