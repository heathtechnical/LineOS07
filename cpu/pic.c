#include<types.h>
#include<system.h>
#include<interrupt.h>
#include<portio.h>
#include<cpu.h>
#include<heap.h>

/* Defined in irq.asm */
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

extern u32 bigi;
extern heap_t k_heap;

void *k_irq_routines[16]=
{
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0
};

void press(struct regs *r)
{
	u32 scancode;

	extern const u32 bigi_1, bigi_2, bigi_3;

	scancode=inportb(0x60);

	if(scancode==0x47){
	    ps();
	}

	if(scancode==0x48){
		schedule();
	}

	if(scancode==0x49){
		dump_heap_stats();
	}

	if(scancode==0x50){
		kprintf("Corrupting Heap!\n");
		memcpy((void *)k_heap.base,0,1024);
	}

}

void init_pic(void)
{
	// TODO: Replace with constants

	/* Remap IRQ's 0-15 to IDT entries 32-47 */
	outportb(0x20, 0x11);
	outportb(0xA0, 0x11);
	outportb(0x21, 0x20);
	outportb(0xA1, 0x28);
	outportb(0x21, 0x04);
	outportb(0xA1, 0x02);
	outportb(0x21, 0x01);
	outportb(0xA1, 0x01);
	outportb(0x21, 0x0);
	outportb(0xA1, 0x0);

	map_interrupt_gate(32,(u32)irq0,0x10,0x8E);
	map_interrupt_gate(33,(u32)irq1,0x10,0x8E);
	map_interrupt_gate(34,(u32)irq2,0x10,0x8E);
	map_interrupt_gate(35,(u32)irq3,0x10,0x8E);
	map_interrupt_gate(36,(u32)irq4,0x10,0x8E);
	map_interrupt_gate(37,(u32)irq5,0x10,0x8E);
	map_interrupt_gate(38,(u32)irq6,0x10,0x8E);
	map_interrupt_gate(39,(u32)irq7,0x10,0x8E);
	map_interrupt_gate(40,(u32)irq8,0x10,0x8E);
	map_interrupt_gate(41,(u32)irq9,0x10,0x8E);
	map_interrupt_gate(42,(u32)irq10,0x10,0x8E);
	map_interrupt_gate(43,(u32)irq11,0x10,0x8E);
	map_interrupt_gate(44,(u32)irq12,0x10,0x8E);
	map_interrupt_gate(45,(u32)irq13,0x10,0x8E);
	map_interrupt_gate(46,(u32)irq14,0x10,0x8E);
	map_interrupt_gate(47,(u32)irq15,0x10,0x8E);
	set_irq_handler(1,&press);
}

void set_irq_handler(u16 irq,void (*handler)(struct regs *r))
{
	k_irq_routines[irq]=handler;
}
