
#include "Isr.h"
#include "Devices/DebugPort/Logger.h"
#include "Pic.h"

static ISRHandler interrupt_dispatcher_vector[NUMBER_OF_IDT_ENTRIES] __attribute__((aligned(4))) = {};

// `interrupt line` means the original hardware interrupt line.
// while `irq` means the  `interrupt line` after adding PIC1_IDT_OFFSET.

void ISR::register_software_interrupt_handler(ISRFunction address, uint8_t irq_number)
{
	ISRHandler handler{.function = address, .type = ISRType::Software};
	interrupt_dispatcher_vector[irq_number] = handler;
}

void ISR::register_hardware_interrupt_handler(ISRFunction address, uint8_t interrupt_line)
{
	PIC::enable_irq(interrupt_line);
	ISRHandler handler{.function = address, .type = ISRType::Hardware};
	interrupt_dispatcher_vector[interrupt_line + PIC1_IDT_OFFSET] = handler;
}

extern "C" void __attribute__((cdecl)) interrupt_dispatcher(ISRContextFrame info)
{
	ISRHandler& handler = interrupt_dispatcher_vector[info.irq_number];
	if (handler.function) {
		handler.function(info);
		if (handler.type == ISRType::Hardware) {
			PIC::acknowledge_pic(info.irq_number);
		}
	} else {
		ISR::default_interrupt_handler(info);
	}
}

void ISR::default_interrupt_handler(ISRContextFrame& info)
{
	if (info.irq_number < 19) {
		err log;
		log << "Exception: \n";
		log << exception_messages[info.irq_number] << "\n";
		log << "Context Dump:\n";
		log << "EIP=" << Hex(info.eip) << "  CS=%X " << Hex(info.cs) << "  ESP=" << Hex(info.context_stack) << "\n";
	} else if (info.irq_number < NUMBER_OF_IDT_EXCEPTIONS) {
		err() << "Reserved Exception Number";
	} else {
		err() << "Undefined IRQ Number (IRQ" << info.irq_number << ")";
	}
	HLT();
}

const char* ISR::exception_messages[19] = {"Division by zero",
                                           "Debug",
                                           "Non-maskable interrupt",
                                           "Breakpoint",
                                           "Detected overflow",
                                           "Out-of-bounds",
                                           "Invalid opcode",
                                           "No coprocessor",
                                           "Double fault",
                                           "Coprocessor segment overrun",
                                           "Bad TSS",
                                           "Segment not present",
                                           "Stack fault",
                                           "General protection fault",
                                           "Page fault",
                                           "Unknown interrupt",
                                           "Coprocessor fault",
                                           "Alignment check",
                                           "Machine check"};