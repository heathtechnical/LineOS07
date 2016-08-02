#include<types.h>
#include<system.h>
#include<stdlib.h>
#include<page.h>
#include<heap.h>
#include<process.h>
#include<cpu.h>
#include<ds/list.h>
#include<string.h>

u32 get_pid(void)
{
	process_t *me;

	me=get_current_process();
	
	return me->pid;
}

u32 get_ppid(void)
{
	process_t *me;
	
	me=get_current_process();

	return me->parent->pid;
}
