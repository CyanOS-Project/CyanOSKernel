#pragma once

#ifdef NDEBUG
    /*
     * If not debugging, assert does nothing.
     */
	#define ASSERT(x)              ((void)0)
	#define ASSERT_NOT_REACHABLE() ((void)0)

#else /* debugging enabled */
	#ifdef CYANOS_KENREL
		#include "Arch/x86/Panic.h"
		#define ASSERT(x)                                                                                              \
			if (!(x)) {                                                                                                \
				PANIC("Null Value ( " #x " )");                                                                        \
			}
		#define ASSERT_NOT_REACHABLE() PANIC("Not Reachable Line")
	#else
		#define ASSERT(x)              ((void)0)
		#define ASSERT_NOT_REACHABLE() ((void)0)
	#endif // CYANOS_KENREL

#endif