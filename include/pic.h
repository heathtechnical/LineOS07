void init_pic(void);
void set_irq_handler(u16 irq,void (*handler)(struct regs *r));
void enable_ints(void);
void disable_ints(void);
