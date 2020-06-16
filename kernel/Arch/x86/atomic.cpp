#include "atomic.h"
#include "Arch/x86/asm.h"
#include "utils/assert.h"

int atomic_level = 0;

void atomic_intiate()
{
	atomic_level = 0;
}
void atomic_begin()
{
	atomic_level++;
	DISABLE_INTERRUPTS();
}

void atomic_end()
{
	atomic_level--;
	if (atomic_level == 0) {
		ENABLE_INTERRUPTS();
	}
	ASSERT(atomic_level > 0);
}