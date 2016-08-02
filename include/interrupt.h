
typedef struct idt_entry{
    u16 base_lo;
    u16 sel;        /* Our kernel segment goes here! */
    u8 always0;     /* This will ALWAYS be set to 0! */
    u8 flags;       /* Set using the above table! */
    u16 base_hi;
} __attribute__((packed)) idt_entry_t;

typedef struct idt_ptr{
    u16 limit;
    u32 base;
} __attribute__((packed)) idt_ptr_t;

/*
	Function Prototypes
*/
void init_interrupts(void);
void map_interrupt_gate(u16 num, u32 virt_base, u16 sel, u8 flags);
