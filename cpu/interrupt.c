#include<system.h>
#include<types.h>
#include<interrupt.h>
#include<portio.h>

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void idt_load();

const extern u32 K_IDT[],K_IDT_PTR[];

/* Global IDT */
idt_entry_t *k_idt;

/* Global IDT */
idt_ptr_t *k_idt_ptr;

void init_interrupts(void)
{
	u32 idt_phys_base, idt_ptr_phys_base;	

	/* Set the k_ pointers */
	k_idt=(idt_entry_t *)K_IDT;
	k_idt_ptr=(idt_ptr_t *)K_IDT_PTR;

	/* Calculate physical address of IDT structures */
	idt_phys_base=(u32)k_idt - (u32)K_ADDR_ADJUST;
	idt_ptr_phys_base=(u32)k_idt_ptr - (u32)K_ADDR_ADJUST;

	/* Populate IDT_PTR */
	k_idt_ptr->limit=(sizeof(idt_entry_t) * 256) - 1;
	k_idt_ptr->base=idt_phys_base;

	map_interrupt_gate(0,(u32)isr0,0x10,0x8E);
	map_interrupt_gate(1,(u32)isr1,0x10,0x8E);
	map_interrupt_gate(2,(u32)isr2,0x10,0x8E);
	map_interrupt_gate(3,(u32)isr3,0x10,0x8E);
	map_interrupt_gate(4,(u32)isr4,0x10,0x8E);
	map_interrupt_gate(5,(u32)isr5,0x10,0x8E);
	map_interrupt_gate(6,(u32)isr6,0x10,0x8E);
	map_interrupt_gate(7,(u32)isr7,0x10,0x8E);
	map_interrupt_gate(8,(u32)isr8,0x10,0x8E);
	map_interrupt_gate(9,(u32)isr9,0x10,0x8E);
	map_interrupt_gate(10,(u32)isr10,0x10,0x8E);
	map_interrupt_gate(11,(u32)isr11,0x10,0x8E);
	map_interrupt_gate(12,(u32)isr12,0x10,0x8E);
	map_interrupt_gate(13,(u32)isr13,0x10,0x8E);
	map_interrupt_gate(14,(u32)isr14,0x10,0x8E);
	map_interrupt_gate(15,(u32)isr15,0x10,0x8E);
	map_interrupt_gate(16,(u32)isr16,0x10,0x8E);
	map_interrupt_gate(17,(u32)isr17,0x10,0x8E);
	map_interrupt_gate(18,(u32)isr18,0x10,0x8E);
	map_interrupt_gate(19,(u32)isr19,0x10,0x8E);
	map_interrupt_gate(20,(u32)isr20,0x10,0x8E);
	map_interrupt_gate(21,(u32)isr21,0x10,0x8E);
	map_interrupt_gate(22,(u32)isr22,0x10,0x8E);
	map_interrupt_gate(23,(u32)isr23,0x10,0x8E);
	map_interrupt_gate(24,(u32)isr24,0x10,0x8E);
	map_interrupt_gate(25,(u32)isr25,0x10,0x8E);
	map_interrupt_gate(26,(u32)isr26,0x10,0x8E);
	map_interrupt_gate(27,(u32)isr27,0x10,0x8E);
	map_interrupt_gate(28,(u32)isr28,0x10,0x8E);
	map_interrupt_gate(29,(u32)isr29,0x10,0x8E);
	map_interrupt_gate(30,(u32)isr30,0x10,0x8E);
	map_interrupt_gate(31,(u32)isr31,0x10,0x8E);

	/* Load IDT */	
	idt_load();
}

void map_interrupt_gate(u16 num, u32 virt_base, u16 sel, u8 flags)
{
	u32 phys_base=(u32)virt_base;
	//-(u32)K_ADDR_ADJUST;

	// TODO: num needs to be bounds checked

	/* Populate base address */
	k_idt[num].base_lo=(phys_base & 0xFFFF);
	k_idt[num].base_hi=(phys_base >> 16) & 0xFFFF;

	/* Populate other fields */	
	k_idt[num].sel=sel;
        k_idt[num].always0=0;
        k_idt[num].flags=flags;
}
