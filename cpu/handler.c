#include<system.h>
#include<types.h>
#include<interrupt.h>
#include<page.h>

extern address_space_t k_addrspc;
extern void *k_irq_routines[16];

void fault_handler(struct regs *r)
{
    /* Is this a fault whose number is from 0 to 31? */

	if(r->int_no==14) page_fault(r);

    if (r->int_no < 32 && r->int_no!=14)
    {
        /* Display the description for the Exception that occurred.
        *  In this tutorial, we will simply halt the system using an
        *  infinite loop */
	kprintf("\n\n************** EXCEPTION ***************\n");
	kprintf("Number: %d, Error Code=%d\n",r->int_no,r->err_code);
	kprintf("Registers: CS:EIP = %x:%x, EAX=%x, EBX=%x, ECX=%x, EDX=%x\n",r->cs,r->eip,r->eax,r->ebx,r->ecx,r->edx);
	kprintf("****************************************\n\n");
        for (;;);
    }
}

void irq_handler(struct regs *r)
{
	void (*handler)(struct regs *r);

	/* Find out if we have a custom handler to run for this
	*  IRQ, and then finally, run it */
	handler = k_irq_routines[r->int_no - 32];
	if (handler)
	{
		handler(r);
	}

	/* If the IDT entry that was invoked was greater than 40
	*  (meaning IRQ8 - 15), then we need to send an EOI to
	*  the slave controller */
	if (r->int_no >= 40)
	{
		outportb(0xA0, 0x20);
	}

	/* In either case, we need to send an EOI to the master
	*  interrupt controller too */
	outportb(0x20, 0x20);
}

void page_fault(struct regs *r)
{
	u32 fault_address;
	u32 phys, vbase;

	/* Get faulting address */
	__asm__("movl %%cr2,%0":"=r" (fault_address));

	phys=(u32)alloc_pages(1,ZONE_NORMAL);

	/* We need to get the base address of the page this address lies in */
	vbase=(fault_address / 4096) * 4096;

	__map_page(&k_addrspc,(void *)phys,(void *)vbase);
	return;
}
