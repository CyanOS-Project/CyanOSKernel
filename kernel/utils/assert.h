#include "Arch/x86/panic.h"
#ifdef NDEBUG
    /*
     * If not debugging, assert does nothing.
     */
	#define ASSERT(x)              ((void)0)
	#define ASSERT_NOT_REACHABLE() ((void)0)

#else /* debugging enabled */

	#define ASSERT(x)                                                                                                  \
		if (!(x)) {                                                                                                    \
			PANIC("Null Value ( " #x " )");                                                                            \
		}
	#define ASSERT_NOT_REACHABLE() PANIC("Not Reachable Line")

#endif