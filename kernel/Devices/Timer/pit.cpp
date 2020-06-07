#include "pit.h"
#include "Arch/x86/asm.h"
#include "Arch/x86/panic.h"
#include "Arch/x86/pic.h"
#include "console.h"

void setup_pit()
{
	enable_irq(PIC_PIT);
	register_isr_handler(pit_handler, PIC_PIT + PIC1_IDT_OFFSET);
	out8(I86_PIT_REG_COMMAND, I86_PIT_OCW_COUNTER_0 | I86_PIT_OCW_RL_DATA | I86_PIT_OCW_MODE_SQUAREWAVEGEN);
	out8(I86_PIT_REG_COUNTER0, 0xFF); // Lower Half
	out8(I86_PIT_REG_COUNTER0, 0xFF); // Higher Half
}

void pit_handler(ISR_INFO isr_info)
{
	printf("hello\n");
	acknowledge_pic(PIC_PIT);
}