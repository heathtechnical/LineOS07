#include<system.h>
#include<types.h>
#include<page.h>
#include<multiboot.h>
#include<tty.h>
#include<interrupt.h>
#include<heap.h>
#include<cpu.h>
#include<process.h>
#include<devmgr.h>
#include<floppy.h>

void threada(void);
void threadb(void);

void panic(char *msg)
{
	kprintf("PANIC! - %s\n",msg);
	__asm__ __volatile__ ("hlt");

}

u32 bigi_1=1;
u32 bigi_2=0;
u32 bigi_3=0;

void task1(void)
{
	process_sleep(5000);
	kprintf("Ending process now!\n");
	process_end();
}

void task2(void)
{
	while(1){
		kprintf("");
		bigi_2+=1;

	}
}

void task3(void)
{
	while(1){
		kprintf("");
		bigi_3+=1;
	}
}

void banner(void){
kprintf(" _ _         _____ _____ \n");
kprintf("| |_|___ ___|     |   __|\n");
kprintf("| | |   | -_|  |  |__   |\n");
kprintf("|_|_|_|_|___|_____|_____|\n");
kprintf("================================================================================");
}

int main()
{
/*	const extern u32 _KERNEL_START[],_KERNEL_END[];
	const extern u32 K_CODE_SEL[], K_DATA_SEL[];
	const extern u32 _KERNEL_TEXT_START[],_KERNEL_TEXT_END[];
	const extern u32 _KERNEL_DATA_START[],_KERNEL_DATA_END[];
	const extern u32 _KERNEL_BSS_START[],_KERNEL_BSS_END[]; */

	tty_cls();

	banner();

	/*if(TEST_BIT(K_MULTIBOOT_INFO->flags,MBI_FLAGS_MMAP)){
		kprintf("MMAP Present: 0x%x\n",K_MULTIBOOT_INFO->mmap_addr);

		memory_map_t *mmap;
		for(mmap=(memory_map_t *)K_MULTIBOOT_INFO->mmap_addr;
			(u32)mmap<K_MULTIBOOT_INFO->mmap_addr + K_MULTIBOOT_INFO->mmap_length;
			mmap=(memory_map_t *)((u32)mmap+mmap->size+sizeof(mmap->size))){
			kprintf("[%d] addr=0x%x-0x%x\n",mmap->type,mmap->base_addr_low,(mmap->base_addr_low+mmap->length_low));

		}
	}*/

	kprintf(" > Initialising system services...\n");
	init_interrupts();
	init_paging();
	init_heap();
	set_cpu_caps();
	init_pic();
	init_timer();
	init_process();

	kprintf(" > Enabling kernel preemption...\n");
	enable_kernel_preempt();
	enable_ints();
	
	kprintf(" > Spawning core process...\n");

	create_process("core",NULL,&core);


	/* We may get to here for a very short amount of time while we enable preemption */
	while(1);
}

void core(void)
{
	kprintf("   * Core process starting\n");
	
	kprintf("   * Initialising device manager...\n");
	init_devices();

	kprintf("   * Detecting hardware...\n");
	init_floppy();

	//create_process("heap",NULL,&heapchecker);
	// TODO: This should hlt
	while(1);
}
