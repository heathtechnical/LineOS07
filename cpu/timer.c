#include<types.h>
#include<system.h>
#include<portio.h>
#include<timer.h>
#include<pic.h>
#include<ds/list.h>
#include<process.h>

void init_timer(void)
{
	u32 divisor = 1193180 / TIMER_HZ;

	/* Set Timer HZ */
	outportb(0x43, 0x36);
	outportb(0x40, divisor & 0xFF);
	outportb(0x40, divisor >> 8);

	set_irq_handler(0,&timer_handler);
}

void timer_handler(struct regs *r)
{
 	schedule();
}
