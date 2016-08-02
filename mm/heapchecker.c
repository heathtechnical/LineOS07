#include<types.h>
#include<system.h>
#include<page.h>
#include<heap.h>
#include<ds/list.h>

extern system_memory_t k_sysmem;

extern heap_t k_heap;

extern malloc_chain_t k_malloc_chain;

void heapchecker(void)
{
	malloc_head_t *ptr=NULL;
	struct list_head *this;

	while(1){

		process_sleep(500);

		/* Walk the heap every so often, checking for corruption */
        	list_for_each(this,&k_malloc_chain.chain){
                	ptr=list_entry(this,malloc_head_t,list);

			if(ptr->magic!=0xBEEF){
				panic("HEAP BLOCK CORRUPTION DETECTED!");
			}
		}
	}

}
