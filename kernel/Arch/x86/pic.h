#include "asm.h"
#include "utils/types.h"

#define PIC1_BASE       (0x20)
#define PIC2_BASE       (0xA0)
#define PIC1_COMMAND    (PIC1_BASE)
#define PIC1_DATA       (PIC1_BASE + 1)
#define PIC2_COMMAND    (PIC2_BASE)
#define PIC2_DATA       (PIC2_BASE + 1)
#define PIC1_IDT_OFFSET (0x20)
#define PIC2_IDT_OFFSET (0x28)

#define ICW1_ICW4       0x01
#define ICW1_SINGLE     0x02
#define ICW1_INTERVAL4  0x04
#define ICW1_LEVEL      0x08
#define ICW1_INIT       0x10
#define ICW4_8086       0x01
#define ICW4_AUTO       0x02
#define ICW4_BUF_SLAVE  0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM       0x10
#define SLAVE_IRQ2      0x4

#define PIC_PIT      0
#define PIC_KEYBOARD 1
#define PIC_CASCADE  2
#define PIC_COM2     3
#define PIC_COM1     4
#define PIC_MOUSE    12

#define PIC_EOI 0x20

class PIC
{
  private:
	/* data */
  public:
	static void setup();
	static void enable_irq(uint8_t irq);
	static void acknowledge_pic(uint8_t irq);
};
