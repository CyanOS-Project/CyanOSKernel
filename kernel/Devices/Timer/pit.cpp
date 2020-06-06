#include "pit.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/panic.h"
#include "Arch/x86/pic.h"

void setup_pit()
{
	enable_irq(PIC_PIT);
	register_isr_handler(pit_handler, PIC_PIT + PIC1_IDT_OFFSET);
	out8(I86_PIT_REG_COMMAND, I86_PIT_OCW_COUNTER_0 | I86_PIT_OCW_RL_DATA | I86_PIT_OCW_MODE_SQUAREWAVEGEN);
	out8(I86_PIT_REG_COUNTER0, 0xA9); // Lower Half
	out8(I86_PIT_REG_COUNTER0, 0x04); // Higher Half
}

void pit_handler(ISR_INFO isr_info)
{
	PANIC("PIT is working !");
	acknowledge_pic(PIC_PIT);
}