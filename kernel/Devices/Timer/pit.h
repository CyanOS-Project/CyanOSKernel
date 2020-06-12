#include "Arch/x86/isr.h"

#define I86_PIT_REG_COUNTER0 0x40
#define I86_PIT_REG_COUNTER1 0x41
#define I86_PIT_REG_COUNTER2 0x42
#define I86_PIT_REG_COMMAND  0x43

enum I86_PIT_OCW {
	I86_PIT_OCW_BINCOUNT_BINARY = 0,      // 0		//! Use when setting I86_PIT_OCW_MASK_BINCOUNT
	I86_PIT_OCW_BINCOUNT_BCD = 1,         // 1
	I86_PIT_OCW_MODE_TERMINALCOUNT = 0,   // 0000		//! Use when setting I86_PIT_OCW_MASK_MODE
	I86_PIT_OCW_MODE_ONESHOT = 0x2,       // 0010
	I86_PIT_OCW_MODE_RATEGEN = 0x4,       // 0100
	I86_PIT_OCW_MODE_SQUAREWAVEGEN = 0x6, // 0110
	I86_PIT_OCW_MODE_SOFTWARETRIG = 0x8,  // 1000
	I86_PIT_OCW_MODE_HARDWARETRIG = 0xA,  // 1010
	I86_PIT_OCW_RL_LATCH = 0,             // 000000	//! Use when setting I86_PIT_OCW_MASK_RL
	I86_PIT_OCW_RL_LSBONLY = 0x10,        // 010000
	I86_PIT_OCW_RL_MSBONLY = 0x20,        // 100000
	I86_PIT_OCW_RL_DATA = 0x30,           // 110000
	I86_PIT_OCW_COUNTER_0 = 0,            // 00000000	//! Use when setting I86_PIT_OCW_MASK_COUNTER
	I86_PIT_OCW_COUNTER_1 = 0x40,         // 01000000
	I86_PIT_OCW_COUNTER_2 = 0x80          // 10000000
};

class PIT
{
  private:
	static void pit_handler(ContextFrame* isr_info);

  public:
	static void setup();
	static void sleep(unsigned Duration);
};
