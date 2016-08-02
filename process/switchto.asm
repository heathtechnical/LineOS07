[extern _k_proclist]
GLOBAL _switchTo

_switchTo:
	; Eax now has our return address
	pop eax

	; Get address of current process
	;mov ebx, [_k_proclist]

	; Push EFLAGS
	pushfd
	
	; Push Code Segment
	push cs

	; Push Return Address
	push eax

	; Push Segment Registers
	push es
	push ds
	push fs
	push gs
	pushad

	; Save current esp - If we don't have a current process
	; esp will get saved at [0x0]
	mov eax, [_k_proclist]

	cmp eax, 0
	jz first_task

	mov [eax], esp

   first_task:

	; Get new process structure - Remember this was passed to us
	; as arg1, so its way down the stack past what we just pushed
	; We actually pushed 15 4byte words, so we want esp+60
	mov eax, [esp+60]

	; Now set k_proclist.current pointer to point at our new
	; process
	mov [_k_proclist],eax

	; At this point, esp still contains the old stack, we need to flick it over
	; to newproc.esp, or k_proclist.current.esp, arg1.esp or even [esp+60].esp
	; Because we already have arg1 in eax, we can just do [eax]
	mov esp, [eax]

	; Now we have our new processes stack in esp, we can reload the process's
	; state (if it has one, otherwise, it will get the default values.
	popad
	pop gs
	pop fs
	pop ds
	pop es

	; At this point, we have restored the general purpose registers, and stack registers
	; all that is left on the stack is EIP, CS, and EFLAGS, which iret will pop off, restore
	; and we will then be running in our desired location

	iret
