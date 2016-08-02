#include<ds/list.h>

#define KMALLOC_FREE 0x0
#define KMALLOC_USED 0x1

#define KM_PGALIGN 0x4

typedef struct heap{
	void *base;
	void *brk;
} heap_t;

typedef struct malloc_head{
	struct list_head list;
	u32 magic;
	u32 size;
	u8 state;
}malloc_head_t;

typedef struct malloc_chain{
	struct list_head chain;
	u32 total_blocks;
	u32 used_blocks;
}malloc_chain_t;

typedef struct temp_map{
	struct list_head list;
	void *paddr;
	u32 pages;
}temp_map_t;

/*
	Function prototypes
*/
void init_heap(void);
s8 kbrk(void *end);
void *ksbrk(u32 bytes);
void *kmalloc(u32 bytes,u16 flags);
void kfree(void *base);
void heapchecker(void);
void *map_temp(u32 paddr, u32 pages);
