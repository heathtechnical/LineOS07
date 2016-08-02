#include<types.h>
#include<system.h>
#include<page.h>
#include<heap.h>
#include<ds/list.h>
#include<process.h>

void threada(void)
{
    while(1) kprintf("a");
    process_end();
}

void threadb(void)
{
    while(1) kprintf("b");
    process_end();
}