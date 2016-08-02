[BITS 32]
%define ADDRADJUST   0xBFF00000

; // Kernel virtual start address //
%define K_VIR_START   0xC0000000

; // Kernel physiscal start address //
%define K_PHYS_START   0x100000

; // Addressing macros //
%define SEGMENT(linear)   ((linear & 0xFFFF0000) >> 4)
%define OFFSET(linear)   (linear & 0xFFFF)

; // Paging constants //
%define P_PRESENT    0x01
%define P_WRITE      0x02
%define P_USER      0x04
%define PAGE_SIZE   0x1000

; // Global variables //
GLOBAL _start
[EXTERN _main]
[EXTERN K_STACK_END]
GLOBAL _K_DATA_SEL
GLOBAL _K_CODE_SEL
GLOBAL _K_TSS_SEL
GLOBAL _K_TSS
GLOBAL _K_PD
GLOBAL _I_PT
GLOBAL _K_PT
GLOBAL _K_IDT
GLOBAL _K_IDT_PTR
GLOBAL _K_MULTIBOOT_INFO
GLOBAL gdtr

_start:
	; Preserve multiboot information structure
	mov dword [(_K_MULTIBOOT_INFO - ADDRADJUST)],ebx
	jmp   entry

	MULTIBOOT_PAGE_ALIGN equ (1 << 0)
	MULTIBOOT_MEMORY_INFO equ (1 << 1)

	MULTIBOOT_HEADER_MAGIC equ (0x1BADB002)
	MULTIBOOT_HEADER_FLAGS equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
	CHECKSUM equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

	ALIGN 4
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd CHECKSUM

entry:

	; Turn off floppy motor  - Is this really needed?
	mov   dx, 0x03F2
	mov   al, 0x0C
	out   dx, al

	; Populate identity map PT
	mov   eax, (P_PRESENT | P_WRITE)
	mov   ecx, 1024
	mov   edi, (_I_PT - ADDRADJUST)
	cld
	fill_I_PT:
		stosd
		add   eax, PAGE_SIZE
	loop   fill_I_PT

	; Populate kernel virtual map
	mov   eax, (K_PHYS_START | P_PRESENT | P_WRITE)
	mov   ecx, 1024
	mov   edi, (_K_PT - ADDRADJUST)
	fill_K_PT:
		stosd
		add   eax, PAGE_SIZE
	loop   fill_K_PT

	; Create identity map entry in page directory
	mov eax, (_I_PT - ADDRADJUST)
	or eax, P_PRESENT
	or eax, P_WRITE
	or eax, P_USER
	mov dword [(_K_PD - ADDRADJUST)], eax

	; Create kernel virtual map entry in page directory
	mov eax, (_K_PT - ADDRADJUST)
	or eax, P_PRESENT
	or eax, P_WRITE
	or eax, P_USER
	mov dword [(_K_PD - ADDRADJUST)+(K_VIR_START/(PAGE_SIZE*1024))*4], eax

	; Create self map entry - Is this really needed?
	mov eax, (_K_PD - ADDRADJUST)
	or eax, P_PRESENT
	or eax, P_WRITE
	or eax, P_USER
	mov dword [(_K_PD - ADDRADJUST)+1023*4], eax


	; Load page directory into PDBR (cr3)
	mov   eax, (_K_PD - ADDRADJUST)
	mov   cr3, eax

	; Enable Paging
	mov   eax, cr0
	or   eax, 0x80010000 ; CR0.PG = CR0.WP = 1
	mov   cr0, eax

	; Load GDTR
	o32 a32 lgdt [gdtr]

	; Far jmp to reload cs:eip
	jmp   _K_CODE_SEL:_K_STAGE2

_K_STAGE2:
	; Reload data segment registers
	mov   eax, _K_DATA_SEL
	mov   ds, eax
	mov   es, eax
	mov   ss, eax
	nop
	mov   fs, eax
	mov   gs, eax

	; For now the stack is defined in link.ld, this will probebly change later 
	mov   esp, K_STACK_END

	; Call kernel proper
	call _main

	; If we ever exit from the kernel proper, halt the system
	hlt

; Loads the IDT defined in '_idtp' into the processor.
; This is declared in C as 'extern void idt_load();'
global _idt_load
extern _k_idt_ptr
_idt_load:
    lidt [(_K_IDT_PTR-ADDRADJUST)]
    ret

; Setup default exception handlers (0-32)
%include "cpu/isr.asm"
%include "cpu/irq.asm"
%include "process/switchto.asm"


; This probably keeds moving to our data segment
gdtr:
   dw (gdt_end - gdt - 1)   ; GDT limit
   dd gdt-ADDRADJUST   ; GDT linear address
gdt:
; Dummy descriptor      ; 0x00
   dw 0         ; Limit 15:0
   dw 0         ; Base 15:0
   db 0         ; Base 23:16
   db 0         ; Access byte (descriptor type)
   db 0         ; Limits 19:16, Flags
   db 0         ; Base 31:24

; Data descriptor
_K_DATA_SEL   equ    $-gdt   ; 0x08
   dw 0xFFFF      ; Limit Bits 15:0
   dw 0           ; Base Bits 15:0
   db 0           ; Base Bits 23:16
   db 0x92        ; Access Byte - A=0, W=1, E=0, 0, 1, DPL=00, P=1
   db 0xCF        ; Flags Byte - Limit=1111, AVL=0, 0, D=1 G=1
   db 0           ; Base Bits 31:24

; Code descriptor
_K_CODE_SEL   equ   $-gdt   ; 0x10
   dw 0xFFFF      ; Limit Bits 15:0
   dw 0           ; Base Bits 15:0
   db 0           ; Base Bits 23:16
   db 0x9A        ; Access Byte - A=0, R=1, C=0, 1, 1, DPL=00, P=1
   db 0xCF        ; Flags Byte - Limit=1111, AVL=0, 0, D=1, G=1
   db 0           ; Base Bits 31:24

_K_TSS:
_K_TSS_SEL    equ   $-gdt   ; 0x18
   dw 0
   dw 0
   db 0
   db 0x89
   db 0
   db 0
gdt_end:

SECTION .data
ALIGN 4096
_K_PD:
	times 1024 dd 0
_I_PT:
	times 1024 dd 0
_K_PT:
	times 1024 dd 0
_K_IDT:
	times 2048 db 0
_K_IDT_PTR:
	times 6 db 0
_K_MULTIBOOT_INFO:
	dd 0
