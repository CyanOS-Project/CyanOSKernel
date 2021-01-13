#include <Types.h>

#define KERNEL_SPACE_SIZE       0x40000000
#define KERNEL_BASE             0xC0000000
#define KERNEL_PHYSICAL_ADDRESS 0x00100000
#define KERNEL_VIRTUAL_ADDRESS  (KERNEL_BASE + KERNEL_PHYSICAL_ADDRESS)
#define VIR_TO_PHY(x)           (x - KERNEL_BASE)

extern "C" uintptr_t _KERNEL_START;
extern "C" uintptr_t _KERNEL_END;