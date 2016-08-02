#include<types.h>
#include<system.h>
#include<stdlib.h>
#include<page.h>
#include<heap.h>
#include<process.h>
#include<cpu.h>
#include<ds/list.h>
#include<string.h>

tss_t *k_system_tss;
proclist_t k_proclist;

extern system_memory_t k_sysmem;

/*
	Initialise Multi Processing (task switching) Environment
*/
void init_process(void)
{
	extern const u16 K_TSS_SEL[];
	extern const u32 K_STACK_END[];
	
	process_t *k_main;
	
//	k_system_tss=(tss_t *)kmalloc(sizeof(tss_t),0);
//	if(k_system_tss==NULL){
//		panic("Unable to allocate k_system_tss!");
//	}

//	memset(k_system_tss,0,sizeof(tss_t));

//	set_tss(k_system_tss,sizeof(tss_t));
//	ltr((u16)K_TSS_SEL);

	/* Init k_proclist list head */
	INIT_LIST_HEAD(&k_proclist.list);

	/* Set default current process */
	k_proclist.current=0;

	/* Set default pid */
	k_proclist.pidcnt=0;
	
	/* Set current process count */
	k_proclist.count=0;

	/* Set Kernel Preemption */
	k_proclist.kernel_preempt=0;
}

/*
	Enable Kernel Preemption
*/
void enable_kernel_preempt(void)
{
	k_proclist.kernel_preempt=1;
}

/*
	Disable Kernel Preemption
*/
void disable_kernel_preempt(void)
{
	k_proclist.kernel_preempt=0;
}

/*
	Return Kernel Preemption State
*/
u8 is_kernel_preempt(void)
{
	return k_proclist.kernel_preempt;
}

/*
	Create A Kernel Process
*/
u32 create_process(char *name, void *arg, void *proc)
{
	process_t *newproc, *me;
	u32 i;

	/* Allocate new process_t structure */
	newproc=(process_t *)kmalloc(sizeof(process_t),0);
	if(newproc==NULL){
		panic("Unable to malloc, panicing - this shouldn't be here");
		return NULL;
	}

	/* Zero new process structure */
	memset(newproc,0,sizeof(process_t));

	/* Get & set new PID */
	newproc->pid=k_proclist.pidcnt++;

	/* Set process name */
	strncpy(newproc->name,name,32-1);

	/* Allocate a page_size stack - Does this need to be aligned? */	
	newproc->kstack=(u32)kmalloc(k_sysmem.page_size,0);
	if(newproc->kstack==NULL){
		kfree(newproc);
		panic("Couldn't malloc new stack, panicing - this shouldn't be here");
	}

	/* Zero stack */
	memset((void *)newproc->kstack,0,k_sysmem.page_size);

	/* Set esp, kstack to new stack */
	newproc->esp=newproc->kstack + (k_sysmem.page_size-1);

	/* Add new process to process list */
	list_add_tail(&newproc->list,&k_proclist.list);

	/* Populate new stack */
	setup_kernel_process(newproc,proc);

	/* Increment active processes counter */
	k_proclist.count++;

	/* Give this process some ticks */
	newproc->ticks=100;

	/* Set total ticks */
	newproc->tt=0;

	/* Set Parent */
	newproc->parent=get_current_process();
	
	/* Return our new pid */
	return newproc->pid;
}

/*
	Return Current Process Structure
*/
process_t *get_current_process(void)
{
	return k_proclist.current;
}

/*
	Terminate Current Process
*/
void process_end(void)
{
	process_t *me;

	/* Get running process */
	me=get_current_process();

	/* Remove this process from the active process list */
	list_del(&me->list);

	kfree(me);

	process_expire();
}

/*
	Expire A Process' Timeslice
*/
void process_expire(void)
{
	process_t *me;

	/* Get Current Process */
	me=get_current_process();

	/* Expire the current process' ticks */
	me->ticks=0;

	/* Disable Interrupts */
	disable_ints();
	
	/* Force a re-schedule() */
	schedule();
	
	/* Enable Interrupts */
	enable_ints();
}

/*
	Setup A Kernel Process
*/
void setup_kernel_process(process_t *p, void *proc)
{
	u32 *procstack;

	// TODO: Replace 0x10, and 0x08 with defines
	// TODO: Do this a different way, this way is messy

	/* Set tempoary stack pointer */
	procstack=(u32 *)p->esp;

	/* Push address of process structure */
	*procstack=(u32)p;

	/* Push Eflags TODO: Need to work this one out */
	procstack--;
	*procstack=0x202;

	/* Push kernel code segment */
	procstack--;
	*procstack=0x10;

	/* Push address of function */
	procstack--;
	*procstack=(u32)proc;

	/* Push segment registers */
	procstack--;
	*procstack=0x08;	/* es */

	procstack--;
	*procstack=0x08;	/* ds */

	procstack--;
	*procstack=0x08;	/* fs */

	procstack--;
	*procstack=0x08;	/* gs */

	/* Push General Purpose Registers */
	procstack--;
	*procstack=0x00;	/* eax */

	procstack--;
	*procstack=0x00;	/* ecx */

	procstack--;
	*procstack=0x00;	/* edx */

	procstack--;
	*procstack=0x00;	/* ebx */

	procstack--;
	*procstack=0x00;	/* esp */

	procstack--;
	*procstack=0x00;	/* ebp */

	procstack--;
	*procstack=0x08;	/* esi */

	procstack--;
	*procstack=0x08;	/* edi */

	/* Set process esp */
	p->esp=(u32)procstack;

	/* Set process ss */
	p->ss=0x08;

	p->state=PROC_READY;
}

void ps(void)
{
	/* Walk process list */
        process_t *ptr=NULL;
        struct list_head *this;

	kprintf("---[ Process List ]---");

	if(!is_kernel_preempt()) kprintf("*\n");
		else kprintf("\n");

	kprintf("pid/name/stack/esp/state/ticks\n");
        list_for_each(this,&k_proclist.list){
                ptr=list_entry(this,process_t,list);

		kprintf("%d/%s/0x%x/0x%x/0x%x/%d\n",ptr->pid,ptr->name, ptr->kstack, ptr->esp, ptr->state, ptr->tt);
        }
	kprintf("\n");

}

process_t *select_next_process(void)
{
	process_t *ptr, *selected=NULL;
	struct list_head *this;
	
	findproc:
	/* Walk the process list */
	list_for_each(this,&k_proclist.list){
		ptr=list_entry(this,process_t,list);
		
		/* If this task is elegible to run */
		if(ptr->state==PROC_READY && ptr->ticks>0){
			selected=ptr;
			break;
		}else{
			selected=NULL;
		}
	}
	
	if(selected==NULL){
		/* Give each process back some ticks */
		give_ticks();
		goto findproc;
	}
	
	return selected;
}

void update_timeouts(void)
{
        process_t *ptr, *selected=NULL;
        struct list_head *this;

        /* Walk the process list */
        list_for_each(this,&k_proclist.list){
                ptr=list_entry(this,process_t,list);
		if(ptr->state==PROC_SLEEPING){
			ptr->timeout--;
			if(!ptr->timeout){
				/* Time to wake this process */
				ptr->state=PROC_READY;
			}
		}
	}

}

void process_sleep(u32 ticks)
{
	process_t *me;

	/* Get Current Process */
	me=get_current_process();

	/* Set process to sleeping */
	me->state=PROC_SLEEPING;

	/* Set timeout ticks */
	me->timeout=ticks;

	/* Expire our remaining timeslice */
	process_expire();
	
}

void account_ticks(process_t *process)
{
	if(process->ticks>0){
		--process->ticks;
		process->tt++;
	}
}

void schedule(void)
{
    process_t *ptr=NULL, *nextproc;
	struct list_head *start;
    struct list_head *this;

	/* Return if preemption is disabled */
	if(!is_kernel_preempt()) return;

	/* Return if we have no processes to run */
	if(!k_proclist.count) return;

	update_timeouts();

	/* Return if the current process still has ticks */
	if(k_proclist.current && k_proclist.current->ticks){
		/* Account ticks */
		account_ticks(k_proclist.current);
		return;
	}

	/* Set currently running process to ready */	
	if(k_proclist.current->state==PROC_RUNNING){
		k_proclist.current->state=PROC_READY;
	}


	/* This is horrible and messy, but it will do for now */
	nextproc=select_next_process();
	
	/* Set the new process to running */	
	nextproc->state=PROC_RUNNING;

	/* Tell the PIC we're done */
	outportb(0x20, 0x20);

	/* Increase new proceses ticks */
	account_ticks(nextproc);

	switchTo(nextproc);
}

void give_ticks(void)
{
	struct list_head *this;
	process_t *ptr;

        list_for_each(this,&k_proclist.list){
                ptr=list_entry(this,process_t,list);
		ptr->ticks=100;
        }
}
