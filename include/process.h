
#define PROC_READY    0x1
#define PROC_RUNNING  0x2
#define PROC_SLEEPING 0x3

/*
	Structures
*/
typedef struct process{
	u32 esp;
	u32 ss;
	u32 kstack;
	u32 ustack;	
	u32 cr3;
	u32 pid;
	struct list_head list;
	char name[32];
	u8 state;
	u16 ticks;
	u32 timeout;
	s8 priority;
	struct process_t *parent;
	u32 tt;
}process_t;

typedef struct proclist{
	process_t *current;
	u32 pidcnt;
	u32 count;
	u32 kernel_preempt;
	struct list_head list;
}proclist_t;

/*
	Function Prototypes
*/
void init_process(void);
void ps(void);
process_t *get_current_process(void);
