#pragma once
#define ATEXIT_FUNC_MAX 128

extern "C" {

typedef unsigned uarch_t;

struct atexitFuncEntry_t {
	void (*destructorFunc)(void*);
	void* objPtr;
	void* dsoHandle;
};

extern void* __dso_handle;

int __cxa_atexit(void (*f)(void*), void* objptr, void* dso);
void __cxa_finalize(void* f);

void __cxa_pure_virtual();
};
