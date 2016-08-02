#include<types.h>
#include<page.h>
#include<system.h>

extern system_memory_t k_sysmem;

void test_alloc_pages()
{
	u32 page=0;
	u32 cnt=0, i=1;

	cnt=0;
	kprintf("%s: calling alloc_pages(%d, ZONE_DMA) until NULL...\n",__FUNCTION__,i);
	while(1){
		page=(u32)alloc_pages(i,ZONE_DMA);
		if(page!=NULL) cnt++;
			else break;
	}
	kprintf("%s: done, %d calls, ",__FUNCTION__,cnt);
	if((k_sysmem.page_zones[0].end_page-k_sysmem.page_zones[0].base_page)==k_sysmem.page_zones[0].usedpages){
		kprintf("counters match!\n");
	}else{
		kprintf("counters *DONT* match! (%d)\n",(k_sysmem.page_zones[0].end_page-k_sysmem.page_zones[0].base_page)-k_sysmem.page_zones[0].usedpages);
	}

        cnt=0;
        kprintf("%s: calling alloc_pages(%d, ZONE_NORMAL) until NULL...\n",__FUNCTION__,i);
        while(1){
                page=(u32)alloc_pages(i,ZONE_NORMAL);
                if(page!=NULL) cnt++;
                        else break;
        }
        kprintf("%s: done, %d calls, ",__FUNCTION__,cnt);
        if((k_sysmem.page_zones[1].end_page-k_sysmem.page_zones[1].base_page)==k_sysmem.page_zones[1].usedpages){
                kprintf("counters match!\n");
        }else{
                kprintf("counters *DONT* match! (%d)\n",(k_sysmem.page_zones[1].end_page-k_sysmem.page_zones[1].base_page)-k_sysmem.page_zones[1].usedpages);
        }

}
