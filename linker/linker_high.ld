OUTPUT_FORMAT("elf32-i386")

K_VIR_START = 0xC0000000;
K_PHYS_START = 0x10000;
PAGE_SIZE = 0x1000;

ENTRY (start)

SECTIONS
{
   . = 0xC0000000;

   .text : AT(ADDR(.text) - 0xBFF00000)
   {
      __KERNEL_START = .;
      __KERNEL_TEXT_START = .;
      *(.text)
      *(.rodata*)
      . = ALIGN(PAGE_SIZE);
      __KERNEL_TEXT_END = .;
   }

   .data : AT(ADDR(.data) - 0xBFF00000)
   {
      __KERNEL_DATA_START = .;
      *(.data)
      . = ALIGN(PAGE_SIZE);
      __KERNEL_DATA_END = .;
   }

   .bss : AT(ADDR(.bss) - 0xBFF00000)
   {
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
