/*
	Defines
*/
#define EFLAGS_CF    (1<<0)
#define EFLAGS_PF    (1<<2)
#define EFLAGS_AF    (1<<4)
#define EFLAGS_ZF    (1<<6)
#define EFLAGS_SF    (1<<7)
#define EFLAGS_TF    (1<<8)
#define EFLAGS_IF    (1<<9)
#define EFLAGS_DF    (1<<10)
#define EFLAGS_OF    (1<<11)
#define EFLAGS_IOPL  (3<<12)
#define EFLAGS_NT    (1<<14)
#define EFLAGS_RF    (1<<16)
#define EFLAGS_VM    (1<<17)
#define EFLAGS_AC    (1<<18)
#define EFLAGS_VIF   (1<<19)
#define EFLAGS_VIP   (1<<20)
#define EFLAGS_ID    (1<<21)

/*
	Structures
*/
typedef struct cpu_caps{
	u32 model;
	u32 family;
	u32 type;
	u32 brand;
	u32 stepping;
	u32 signature;
	u32 reserved;
	char string[255];
} cpu_caps_t;

typedef struct gdt_entry{
        u16 limit_low;
        u16 base_low;
        u8 base_mid;
        u8 access;
        u8 flags;
        u8 base_high;
}gdt_entry_t;

typedef struct tss
{
        u16 backlink, __blh;
        u32 esp0;
        u16 ss0, __ss0h;
        u32 esp1;
        u16 ss1, __ss1h;
        u32 esp2;
        u16 ss2, __ss2h;
        u32 cr3;
        u32 eip;
        u32 eflags;
        u32 eax, ecx, edx, ebx;
        u32 esp, ebp, esi, edi;
        u16 es, __esh;
        u16 cs, __csh;
        u16 ss, __ssh;
        u16 ds, __dsh;
        u16 fs, __fsh;
        u16 gs, __gsh;
        u16 ldt, __ldth;
        u16 trace, bitmap;
} tss_t;


/*
	Static Inlines
*/
static __inline__ u32 get_eflags(void){
        unsigned eflags;
        __asm__ __volatile__ ("pushfl\npopl %0" : "=r" (eflags));
        return eflags;
}

static __inline__ void set_eflags(unsigned eflags){
        __asm__ __volatile__ ("pushl %0\npopfl" : : "r" (eflags));
}

/*
	Function Prototypes
*/

/* cpu/cpuid.c */
u8 has_cpuid(void);
void set_cpu_caps(void);
void populate_caps_intel(cpu_caps_t *caps);

/* cpu/cpu.c */
void ltr(u16 selector);
void set_tss(tss_t *tss, u32 size);
void enable_ints(void);
void disable_ints(void);

