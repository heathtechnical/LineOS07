#include<types.h>
#include<system.h>
#include<cpu.h>

// TODO: This is horribly unfinished, finish it properly porting from local/cpudet-clean.c

#define cpuid(in,a,b,c,d) __asm__("cpuid":"=a"(a),"=b"(b),"=c"(c),"=d"(d):"a"(in));

cpu_caps_t k_cpu_caps;

char *cpustring_intel[] = {
        "Brand ID Not Supported.",
        "Intel(R) Celeron(R) processor",
        "Intel(R) Pentium(R) III processor",
        "Intel(R) Pentium(R) III Xeon(R) processor",
        "Intel(R) Pentium(R) III processor",
        "Reserved",
        "Mobile Intel(R) Pentium(R) III processor-M",
        "Mobile Intel(R) Celeron(R) processor",
        "Intel(R) Pentium(R) 4 processor",
        "Intel(R) Pentium(R) 4 processor",
        "Intel(R) Celeron(R) processor",
        "Intel(R) Xeon(R) Processor",
        "Intel(R) Xeon(R) processor MP",
        "Reserved",
        "Mobile Intel(R) Pentium(R) 4 processor-M",
        "Mobile Intel(R) Pentium(R) Celeron(R) processor",
        "Reserved",
        "Mobile Genuine Intel(R) processor",
        "Intel(R) Celeron(R) M processor",
        "Mobile Intel(R) Celeron(R) processor",
        "Intel(R) Celeron(R) processor",
        "Mobile Geniune Intel(R) processor",
        "Intel(R) Pentium(R) M processor",
        "Mobile Intel(R) Celeron(R) processor"
};

char *cpustring_intel_other[] = {
        "Reserved",
        "Reserved",
        "Reserved",
        "Intel(R) Celeron(R) processor",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Intel(R) Xeon(R) processor MP",
        "Reserved",
        "Reserved",
        "Intel(R) Xeon(R) processor",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved"
};


/*
	Detect CPUID Presence
*/
u8 has_cpuid(void)
{
	u32 current_eflags;
	u8 retval;

	/* Get the current value of eflags */
	current_eflags=get_eflags();

	/* Set CPUID Bit */
	set_eflags(current_eflags ^ EFLAGS_ID);
	if((get_eflags() ^ current_eflags) & EFLAGS_ID){
		/* We have CPUID */
		retval=1;
	}else{
		/* We don't have CPUID */
		retval=0;
	}

	/* Reset eflags */
	set_eflags(current_eflags);

	/* Return */
	return retval;
}

/*
	Populate this CPU's cap's structure
*/
void set_cpu_caps(void)
{
	u32 dummy,eax,ebx,ecx,edx;
	
	/* Make sure we have the cpuid instruction */
	if(!has_cpuid()){
		// TODO: This needs handling properly
		panic("CPUID instruction not supported, not sure what to do!\n");
	}

	/* Get Vendor String */
	cpuid(0, dummy, ebx, dummy, dummy);

	switch(ebx){
		case 0x756e6547:	/* Intel Magic */
			populate_caps_intel(&k_cpu_caps);
		break;

		default:		/* Unrecognised CPU */
			panic("UNSUPPORTED CPU!");
		break;
	}

}

/*
	Intel specific CPU caps
*/
void populate_caps_intel(cpu_caps_t *caps)
{
	u32 eax,ebx,ecx,edx,dummy,strpos;

	/* Execute CPUID Instruction */
	cpuid(1,eax,ebx,dummy,dummy);

	/* Populate Caps */
	caps->model=(eax >> 4) & 0xf;
	caps->family=(eax >> 8) & 0xf;
	caps->type=(eax >> 12) & 0x3;
	caps->brand=ebx & 0xff;
	caps->stepping=eax & 0xf;
	caps->reserved=eax >> 14;
	caps->signature=eax;

	strpos=0;

	switch(caps->type){
		case 0:
			strncpy(caps->string,"Original OEM ",13);
			strpos+=13;
			break;
		case 1:
			strncpy(caps->string,"Overdrive ",10);
			strpos+=10;
			break;
		case 2:
			strncpy(caps->string,"Dual-capable ",13);
			strpos+=13;
			break;
		case 3:
			strncpy(caps->string,"Reserved ",9);
			strpos+=9;
			break;
	}

	switch(caps->family){
		case 3:
			strncpy(((char*)caps->string+strpos),"i386 ",5);
			break;
		case 4:
			strncpy(((char*)caps->string+strpos),"i486 ",5);
			break;
		case 5:
			strncpy(((char*)caps->string+strpos),"Pentium ",8);
			break;
		case 6:
			strncpy(((char*)caps->string+strpos),"Pentium Pro ",12);
			break;
		case 15:
			strncpy(((char*)caps->string+strpos),"Pentium 4 ",10);
			break;
	}
}
