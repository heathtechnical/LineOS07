
/*
	Multi Boot Information Flags
*/
#define MBI_FLAGS_MEM 		0
#define MBI_FLAGS_BOOTDEVICE	1
#define MBI_FLAGS_CMDLINE 	2
#define MBI_FLAGS_MODS 		3
#define MBI_FLAGS_AOUT_SYMS 	4
#define MBI_FLAGS_ELF_HEADER	5
#define MBI_FLAGS_MMAP		6
#define MBI_FLAGS_DRIVES	7
#define MBI_FLAGS_CFG_TABLE	8
#define MBI_FLAGS_BL_NAME	9
#define MBI_FLAGS_APM_TABLE	10
#define MBI_FLAGS_VBE_INFO	11

typedef struct multiboot_header{
	u32 magic;
	u32 flags;
	u32 checksum;
	u32 header_addr;
	u32 load_addr;
	u32 load_end;
	u32 bss_end;
	u32 entry_addr;
	
} multiboot_header_t;

typedef struct multiboot_info{
	u32 flags;
	u32 mem_lower;
	u32 mem_upper;
	u32 boot_device;
	u32 cmdline;
	u32 mods_count;
	u32 mods_addr;
	u32 null1;
	u32 null2;
	u32 null3;
	u32 null4;
	u32 mmap_length;
	u32 mmap_addr;
} multiboot_info_t;

typedef struct memory_map{
	u32 size;
	u32 base_addr_low;
	u32 base_addr_high;
	u32 length_low;
	u32 length_high;
	u32 type;
} memory_map_t;     
