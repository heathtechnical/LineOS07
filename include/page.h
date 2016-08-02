/*
	Page Directory/Table Entry Macros
*/
#define PE_MASK_FLAGS(addr) ((((unsigned long)addr) >> 12)<< 12)	/* Mask Page Entry Flags */
#define GET_PE(table,entry) (table * 1024 + entry)			/* Get Page Entry Index */
#define VADDR_PTE(addr) (((addr) >> 12) & 0x3FF)			/* Get Page Table Index From Virtual Address */
#define VADDR_PDE(addr) (((addr) >> 22) & 0x3FF)			/* Get Page Directory Index From Virtual Address */
#define VADDR_FROM_INDEX(table,entry) (((table * 4096) + (entry * 4))*1024)
#define PADDR_BIT_INDEX(addr) ((addr) >> 12)				/* Get Physical Page Bit Index */
#define VADDR_BASE(pd_index) ((pd_index) << 22)				/* Get Virtual Base Address From Page Directory Index */

/*
	Allocation Flags
*/
#define ZONE_DMA      0x00000001
#define ZONE_NORMAL   0x00000002
#define ZONE_HIGHMEM  0x00000004

/*
	Page table flags
*/
#define P_PRESENT    0x01
#define P_WRITE      0x02
#define P_USER      0x04

/*
	Other constants
*/
#define TEMP_RESERVE 4


/*
	Memory Zones
*/
typedef struct page_zone{
	u32 allocation_flag;
	u32 base_page;
	u32 end_page;
	u32 usedpages;
} page_zone_t;


/*
	Address Space Structure
*/
typedef struct address_space{
	u32 *pd;
} address_space_t;

/*
	System Memory Structure
*/
typedef struct system_memory{
	u32 kb_installed;
	page_zone_t *page_zones;
	u8 num_zones;
	u32 total_pages;
	u32 free_pages;
	void *alloc_arg;
	u32 kernel_start;
	u32 kernel_end;
	u16 page_size;
	u32 brk_max;
} system_memory_t;

/*
	Static Functions
*/
static __inline void invlpg(void* page)
{
         __asm__ __volatile__ ("invlpg (%0)": :"r" (page));
}

/*
	Function Prototypes
*/
void init_paging();
void init_page_allocator();
void __mark_pages_used(u32 page,u32 pages);
u32 __get_zone(u32 page);
void *alloc_pages(u32 pages, u32 flags);
void *__map_page(address_space_t *addrspc, void *p, void *v);
void __unmap_page(address_space_t *addrspc, void *v);
u32 get_page_table_entry(address_space_t *addrspc, void *vaddr);
void set_page_table_entry(address_space_t *addrspc, void *vaddr, void *paddr);
u32 get_page_directory_entry(address_space_t *addrspc, void *vaddr);
void set_page_directory_entry(address_space_t *addrspc, void *vaddr, void *paddr);
