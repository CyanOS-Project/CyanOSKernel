#include "pit.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/panic.h"
#include "Arch/x86/pic.h"
#include "Devices/Console/console.h"

void PIT::setup()
{
	PIC::enable_irq(PIC_PIT);
	ISR::register_isr_handler(pit_handler, PIC_PIT + PIC1_IDT_OFFSET);
	out8(I86_PIT_REG_COMMAND, I86_PIT_OCW_COUNTER_0 | I86_PIT_OCW_RL_DATA | I86_PIT_OCW_MODE_SQUAREWAVEGEN);
	out8(I86_PIT_REG_COUNTER0, 0xA9); // Lower Half
	out8(I86_PIT_REG_COUNTER0, 0x04); // Higher Half
}

volatile bool CPU_HLT = false;
volatile unsigned CPU_HLT_DURATION = 0;

void PIT::sleep(unsigned Duration)
{
	CPU_HLT = true;
	CPU_HLT_DURATION = Duration / 2;
	while (CPU_HLT)
		HLT();
}
void PIT::pit_handler(ISR_INFO isr_info)
{
	if (CPU_HLT == true) {
		CPU_HLT_DURATION -= 1;
		if (CPU_HLT_DURATION == 0) {
			CPU_HLT = false;
		}
	}
	// printf("hello\n");
	PIC::acknowledge_pic(PIC_PIT);
}