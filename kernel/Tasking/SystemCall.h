#pragma once
#include "Arch/x86/context.h"
#include "Arch/x86/isr.h"
#include "Devices/Console/console.h"
#include "utils/types.h"

class SystemCall
{
  private:
	static void systemcall_handler(ISRContextFrame* frame);

  public:
	static void setup();
};