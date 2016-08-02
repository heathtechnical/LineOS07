#include<types.h>
#include<system.h>
#include<cpu.h>

/*
	Load Task Register
*/
void ltr(u16 selector)
{
        __asm__ __volatile__("ltr %0" : : "rm"(selector));
}

/*
	Disable Interrupts
*/
void enable_ints(void)
{
        __asm__ __volatile__ ("sti");
}

/*
	Enable Interupts
*/
void disable_ints(void)
{
        __asm__ __volatile__ ("cli");
}


/*
	Set TSS In GDT
*/
void set_tss(tss_t *tss, u32 size)
{
        u32 base=(u32)tss;

        extern gdt_entry_t K_TSS;

        K_TSS.base_low=base & 0xFFFF;
        K_TSS.base_mid=(base >> 16) & 0xFF;
        K_TSS.base_high=(base >> 24) & 0xFF;

        K_TSS.limit_low=size & 0xffff;
        K_TSS.flags |= ((size >> 16) & 0xF);
}
