extern "C" {

#include "icxxabi.h"
#include "../types.h"

atexitFuncEntry_t __atexitFuncs[ATEXIT_FUNC_MAX];
uarch_t __atexitFuncCount = 0;

void* __dso_handle = 0;

int __cxa_atexit(void (*f)(void*), void* objptr, void* dso)
{
	if (__atexitFuncCount >= ATEXIT_FUNC_MAX) {
		return -1;
	}
	__atexitFuncs[__atexitFuncCount].destructorFunc = f;
	__atexitFuncs[__atexitFuncCount].objPtr = objptr;
	__atexitFuncs[__atexitFuncCount].dsoHandle = dso;
	__atexitFuncCount++;
	return 0;
}

void __cxa_finalize(void* f)
{
	signed i = __atexitFuncCount;
	if (!f) {
		while (i--) {
			if (__atexitFuncs[i].destructorFunc) {
				(*__atexitFuncs[i].destructorFunc)(__atexitFuncs[i].objPtr);
			}
		}
		return;
	}

	for (; i >= 0; i--) {
		if (reinterpret_cast<void*>(__atexitFuncs[i].destructorFunc) == f) {
			(*__atexitFuncs[i].destructorFunc)(__atexitFuncs[i].objPtr);
			__atexitFuncs[i].destructorFunc = 0;
		}
	}
}

void __cxa_pure_virtual()
{
	// PANIC("Virtual function has no implementation!");
}

void atexit()
{
	// PANIC("Exiting!");
}
}