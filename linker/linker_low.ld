/* let the linker use its 'Anative' format (ELF/COFF/PE)*/
OUTPUT_FORMAT("elf32-i386")
/* no leading underscore for symbols handled in asm: */
ENTRY(start)
LS_Phys = 0x100000; /* 1 meg = load (physical) address */
LS_Virt = 0x100000; /* 1 meg = virtual address */
SECTIONS
{
    . = LS_Phys;
    .text : AT(LS_Phys)
    {
	__KERNEL_START = .;
	__KERNEL_TEXT_START = .;
	LS_Code = .;
	code = .; _code = .;
	*(.text)
	*(.rodata*)
	. = ALIGN(4096);
    }
	__KERNEL_TEXT_END =.;
	__KERNEL_DATA_START =.;
    .data : AT(LS_Phys + (LS_Data - LS_Code))
    {
	LS_Data = .;
	data = .; _data = .;
	*(.data)
	. = ALIGN(4096);
    }
	__KERNEL_DATA_END =.;
	__KERNEL_BSS_START =.;
    .bss : AT(LS_Phys + (LS_Bss - LS_Code))
    {
	LS_Bss = .;
	bss = .; _bss = .;
	*(.bss)
	*(COMMON) /* "common" variables */
	. = ALIGN(4096);
    }
	__KERNEL_BSS_END =.;
    .stab :
    {
	*(.stab)
    }
    .stabstr :
    {
	*(.stabstr)
    }
    end = .; _end = .;
	__KERNEL_END =.;
}
