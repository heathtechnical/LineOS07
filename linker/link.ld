/* 
	lineOS - link.ld
*/

OUTPUT_FORMAT("elf32-i386")

K_VIR_START = 0xC0000000;
K_PHYS_START = 0x100000;
K_STACK_SIZE = 0x1000;
PAGE_SIZE = 0x1000;

ENTRY (_start)

SECTIONS
{
   .text K_VIR_START : AT(K_PHYS_START)
   {
      __KERNEL_START = .;
      __KERNEL_TEXT_START = .;
      *(.text)
      *(.rodata*)
      . = ALIGN(PAGE_SIZE);
      __KERNEL_TEXT_END = .;
   }

   .data : AT(K_PHYS_START + (__KERNEL_DATA_START - __KERNEL_TEXT_START))
   {
      __KERNEL_DATA_START = .;
      *(.data)
      . = ALIGN(PAGE_SIZE);
      __KERNEL_DATA_END = .;
   }

   .bss : AT(K_PHYS_START + (__KERNEL_BSS_START - __KERNEL_TEXT_START))
   {
      K_STACK = .;
      _stack = .;
      _K_STACK_START = .;
      . += K_STACK_SIZE;
      _stack_end = .;
      _K_STACK_END = .;
      K_STACK_END = .;
      K_END_STACK = .;

      __KERNEL_BSS_START = .;
      *(.bss)
      *(COMMON)
      . = ALIGN(PAGE_SIZE);
      __KERNEL_BSS_END = .;
   }
   . = ALIGN(PAGE_SIZE);
   KERNEL_TOP = .;
   __KERNEL_END = .;
}
