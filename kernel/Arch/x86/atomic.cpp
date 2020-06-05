#include "atomic.h"
#include "Arch/x86/asm.h"

int atomic_level = 0;

void atomic_begin()
{
	atomic_level++;
	CLI();
}

void atomic_end()
{
	if (atomic_level == 0) {
		STI();
	} else {
		atomic_level--;
	}
}