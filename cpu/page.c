#include<types.h>
#include<system.h>
#include<page.h>
#include<ds/bitmap.h>
#include<stdlib.h>

/*
	System memory object
*/
system_memory_t k_sysmem;

/*
	Allocator Bitmap
*/
bitmap_t k_page_bitmap;

/*
	Kernel address space
*/
address_space_t k_addrspc;

/*
	Page Zones
*/
page_zone_t k_page_zones[3];


/*
	Initialise memory subsystem
*/
void init_paging()
{
	extern u32 _KERNEL_START[],_KERNEL_END[];
	u8 highmem=1;
	u32 clean_from, clean_to, cp;

	/* Zero k_sysmem */
	memset(&k_sysmem,0,sizeof(system_memory_t));

	/* Set System Page Size */
	k_sysmem.page_size=K_PAGE_SIZE;

	/* For now we will populate this manually, in reality we need to get this
	   information elsewhere */
	k_sysmem.kb_installed=(32*1024);

	/* Set sysmem total pages */
	k_sysmem.total_pages=k_sysmem.free_pages=k_sysmem.kb_installed / (k_sysmem.page_size / 1024);

	/* Define DMA zone (0-16MB) */
	k_page_zones[0].allocation_flag=ZONE_DMA;
	k_page_zones[0].base_page=0;
	k_page_zones[0].end_page=0xfff;
	k_page_zones[0].usedpages=0;

	/* Define NORMAL zone (16-(896M+16)) */
	k_page_zones[1].allocation_flag=ZONE_NORMAL;
	k_page_zones[1].base_page=0x1000;

	/* Set NORMAL zone bounds */
	if(k_sysmem.total_pages<=0x36fff){
		k_page_zones[1].end_page=k_sysmem.total_pages;
		highmem=0;
	}else{
		k_page_zones[1].end_page=0x36fff;
	}
	k_page_zones[1].usedpages=0;

	if(highmem){
		/* Define HIGHMEM zones () */
		k_page_zones[2].allocation_flag=ZONE_HIGHMEM;
		k_page_zones[2].base_page=0x37000;
		k_page_zones[2].end_page=k_sysmem.total_pages;
		k_page_zones[2].usedpages=0;
		k_sysmem.num_zones=3;
	}else{
		k_sysmem.num_zones=2;
	}

	/* Link Zone List */
	k_sysmem.page_zones=(page_zone_t *)&k_page_zones;
	
	/* Set kernel start and end points */
	k_sysmem.kernel_start=(u32)_KERNEL_START;
	k_sysmem.kernel_end=(u32)_KERNEL_END;

	/* Point global page directory pointer */
	k_addrspc.pd=(u32 *)VADDR_BASE(1023);

	/* Set Maximum Heap Breakpoint */
	k_sysmem.brk_max=((u32)k_addrspc.pd - (TEMP_RESERVE * k_sysmem.page_size));

	/* Initialise page frame allocator */
	init_page_allocator();

	/* Cleanup last kernel page table */
	clean_from=k_sysmem.kernel_end;		/* We assume kernel_end is k_sysmem.page_size aligned */
	clean_to=k_sysmem.kernel_end + ((((k_sysmem.kernel_end / 0x400000) * 0x400000) + 0x400000)  - k_sysmem.kernel_end);

	for(cp=clean_from;cp<clean_to;cp+=k_sysmem.page_size){
		__unmap_page(&k_addrspc,(void *)cp);
	}
}


/* 
	Initialise page frame allocator 
*/
void init_page_allocator()
{
	u32 bitmap_pages, kphysstart, kphysend, kpages;

	/* Zero the page_bitmap */
	memset(&k_page_bitmap,0,sizeof(bitmap_t));

	/* We need a place to store our data structures, use the end of the kernel */
	
	// TODO: Check k_sysmem.mmap before going any further
	
	k_page_bitmap.base=(u8 *)k_sysmem.kernel_end;
	k_page_bitmap.limit=k_sysmem.kb_installed / (k_sysmem.page_size / 1024);


	/* Set bitmap_pages, we need that later on */
	bitmap_pages=(k_page_bitmap.limit/8) / k_sysmem.page_size;
	if((k_page_bitmap.limit/8) % k_sysmem.page_size) bitmap_pages++;
	
	// TODO: If the kernel gets to 4MB in size, we will cause a PF when getting pages for 
	//       the allocator, this needs checking


	/* Initialise bitmap */	
	bitmap_init(&k_page_bitmap);

	/* Calculate start & end pages of kernel */
	kphysstart=(u32)PE_MASK_FLAGS(get_page_table_entry(&k_addrspc,(void *)k_sysmem.kernel_start));
	kphysend=(u32)PE_MASK_FLAGS(get_page_table_entry(&k_addrspc,(void *)k_sysmem.kernel_end));
	kpages=(kphysend-kphysstart) / k_sysmem.page_size;

	/* Mark kernel pages as used */
	__mark_pages_used(kphysstart / k_sysmem.page_size,kpages);

	/* Mark bitmap as used */
	__mark_pages_used(kphysend / k_sysmem.page_size,bitmap_pages);

	/* Mark zones in memory map (passwd by multiboot) */
	// TODO: This ^^^

	/* Reserve the first MB just for the hell of it */
	__mark_pages_used(0,256);

	/* Increase the end of the kernel pointer */
	k_sysmem.kernel_end+=(bitmap_pages * k_sysmem.page_size);
}

void dump_stats(void)
{
	u32 i=0;
        kprintf("Memory: %d kbytes total, %d pages, %d available\n",k_sysmem.kb_installed, k_sysmem.total_pages, k_sysmem.free_pages);
        for(i=0;i<k_sysmem.num_zones;i++) kprintf("Zone Used Pages: [0x%x] %d \n",k_sysmem.page_zones[i].allocation_flag,k_sysmem.page_zones[i].usedpages);
        kprintf("\n");
}

/*
	Return memory zone
*/
u32 __get_zone(u32 page)
{
	u8 i;

	for(i=0;i<k_sysmem.num_zones;i++)
		if(page>=k_sysmem.page_zones[i].base_page && page <=k_sysmem.page_zones[i].end_page) return k_sysmem.page_zones[i].allocation_flag;

	/* If we get here, the zones haven't been defined properly */
	panic("Zones are not properly defined!");
	return NULL;
}

/*
	Mark page as used, only used in init_page_allocator
*/
void __mark_pages_used(u32 page, u32 pages)
{
	// TODO: This should return if we try and mark a page that is already marked as used

	u32 i,page_flag;
	u8 j;
	for(i=page;i<page+pages;i++){
		page_flag=__get_zone(i);
		for(j=0;j<k_sysmem.num_zones;j++)
			if(page_flag == k_sysmem.page_zones[j].allocation_flag) break;
		k_sysmem.page_zones[j].usedpages++;
		if(bitmap_bitset(&k_page_bitmap,i)==-1) panic("Unable to set bit in bitmap!");
	}
	k_sysmem.free_pages-=pages;
}

/*
	Get a page table entry
*/
u32 get_page_table_entry(address_space_t *addrspc, void *vaddr)
{
	u32 pde,pte,index;
	
	pde=VADDR_PDE((u32)vaddr);
	pte=VADDR_PTE((u32)vaddr);
	index=GET_PE(pde,pte);

	return addrspc->pd[index];
}

/*
	Set a page table entry
*/
void set_page_table_entry(address_space_t *addrspc, void *vaddr, void *paddr)
{
        u32 pde,pte,index;

        pde=VADDR_PDE((u32)vaddr);
        pte=VADDR_PTE((u32)vaddr);
        index=GET_PE(pde,pte);

        addrspc->pd[index]=(u32)paddr;
}

/*
	Get a page directory entry
*/
u32 get_page_directory_entry(address_space_t *addrspc, void *vaddr)
{
	u32 pde,index;

	pde=VADDR_PDE((u32)vaddr);
	index=GET_PE(1023,pde);

	return addrspc->pd[index];
}

/*
	Set a page directory entry
*/
void set_page_directory_entry(address_space_t *addrspc, void *vaddr, void *paddr)
{
	u32 pde,index;

	pde=VADDR_PDE((u32)vaddr);
	index=GET_PE(1023,pde);

	addrspc->pd[index]=(u32)paddr;
}

/*
	Allocate a brand new page
*/
void *alloc_pages(u32 pages, u32 flags)
{
	u32 sp=0,ep=0,i;
	u32 chosen_page=NULL;
	u32 requested_flags=flags;
	u32 pir=0;

	/* Why would anyone request no pages */
	if(!pages) return NULL;

	/* OK, lets find the correct zone bounds */
	for(i=0;i<k_sysmem.num_zones;i++){
		if(requested_flags & k_sysmem.page_zones[i].allocation_flag){
			sp=k_sysmem.page_zones[i].base_page;
			ep=k_sysmem.page_zones[i].end_page;
			break;
		}
	}

	if(sp>k_sysmem.total_pages){
		/* This zone exceeds the bounds of physical memory */
		// TODO: We need to have a fallback flag
		return NULL;
	}

	if(ep>k_sysmem.total_pages){
		ep=k_sysmem.total_pages;
	}

	/* We didnt find any matching zones */
	if(!sp && !ep) return NULL;

	/* Now go through the bitmap from sp to ep, and look for pages free pages */
	for(i=sp;i<ep;i++){
		if(!bitmap_bitget(&k_page_bitmap,i)){
			if(!pir) chosen_page=i;

			pir++;

			if(pir==pages) break;
		}else{
			/* Not enough contiguous pages */
			pir=0;
		}
	}

	/* Out of memory! */
	if(pir!=pages){
		/* Crikey, we couldn't get enough contiguous pages from the chosen zones */
		return NULL;
	}

	/* Mark Page As Used */
	__mark_pages_used(chosen_page,pages);

	/* Return base address of page */	
	return (void *)(chosen_page * 4096);
}

/*
	Map a physical page to a virtual address
*/
void *__map_page(address_space_t *addrspc, void *p, void *v)
{
	u32 pt;

	/* We only want to map aligned addresses */
	if((u32)p % k_sysmem.page_size || (u32)v % k_sysmem.page_size) return NULL;

	/* Get current page directory entry */
	pt=get_page_directory_entry(addrspc,v);

	/* See if we have a page table for this virtual address */
	if(!pt){
		/* Allocate a page for our new page table */
		pt=(u32)alloc_pages(1,ZONE_NORMAL);
		if(!pt){
			panic("Unable to alloc_pages while mapping page!\n");
		}

		/* Link this table into the PD */
		//addrspc->pd[GET_PE(1023,VADDR_PDE((u32)v))]=((u32)pt | P_PRESENT | P_WRITE);
		set_page_directory_entry(addrspc,v,(void *)((u32)pt | P_PRESENT | P_WRITE));

		/* Invalidate TLB entry */
		invlpg((void *)VADDR_FROM_INDEX(1023,VADDR_PDE((u32)v)));

		/* Zero page table */
		memset((char *)VADDR_FROM_INDEX(1023,VADDR_PDE((u32)v)),0,4096);
	}

	// TODO: We need to check the PTE FLAGS!!!
        if(get_page_table_entry(addrspc,v)){
		/* This page is already mapped */
		return NULL;
	}

	//addrspc->pd[GET_PE(VADDR_PDE((u32)v),VADDR_PTE((u32)v))]=((u32)p | P_PRESENT | P_WRITE | P_USER);
	set_page_table_entry(addrspc,v,(void *)((u32)p | P_PRESENT | P_WRITE | P_USER));

	invlpg((void *)v);

	return v;
}

/*
	Unmap virtual address
*/
void __unmap_page(address_space_t *addrspc, void *v)
{
	/* Why would anyone bother doing this */
	if(v==NULL) return;

	/* Is the page present? */
        //if(addrspc->pd[GET_PE(VADDR_PDE((u32)v),VADDR_PTE((u32)v))] | P_PRESENT){
        if(get_page_table_entry(addrspc,v) | P_PRESENT){
		/* Yes, unmap it! */
		//addrspc->pd[GET_PE(VADDR_PDE((u32)v),VADDR_PTE((u32)v))]=0x0;
		set_page_table_entry(addrspc,v,0x0);
		
		
		/* Invalidate TLB */
		invlpg((void *)v);
	}

	return;
}
