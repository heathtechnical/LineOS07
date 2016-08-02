#include<stdarg.h>
#include<low_level_video.h>
#include<_printf.h>

static int kprintf_helper(unsigned c, void **ptr){
	tty_putc(c);
	return 0;
}
		
void kprintf(const char *fmt, ...){
        va_list args;

        va_start(args, fmt);
        (void)do_printf(fmt, args, kprintf_helper, NULL);
        va_end(args);
}
