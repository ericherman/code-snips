/* cores.c : how many physical and hyperthreaded cores do we have?
   Copyright (C) 2018 Eric Herman <eric@freesa.org>
   License: LGPL v2.1 or any later version */

#include <stdio.h>		/* printf */
#include <stdint.h>		/* uint32_t */

#ifdef _WIN32
#include <windows.h>
#elif MACOS
#include <sys/param.h>		/* _SC_NPROCESSORS_ONLN */
#include <sys/sysctl.h>		/* sysconf */
#else
#include <unistd.h>		/* sysconf */
#endif

/* This list may not be complete */
#if defined __X86__ || \
	defined __i386__ || \
	defined i386 || \
	defined __386__ || \
	defined __x86_64__ || \
	defined _M_IX86 || \
	defined _M_X64
#define Cpu_looks_like_x86 1
#define is_hyperthreaded() is_hyperthreaded_x86()
#endif

#ifndef is_hyperthreaded
#define is_hyperthreaded() 0
#endif

unsigned apparent_processors(void)
{
	unsigned procs;

#ifdef _WIN32
	SYSTEM_INFO systeminfo;
	GetSystemInfo(&systeminfo);

	procs = (unsigned)systeminfo.dwNumberOfProcessors;
#else
	procs = (unsigned)sysconf(_SC_NPROCESSORS_ONLN);
#endif

	return procs;
}

#if Cpu_looks_like_x86
int is_hyperthreaded_x86(void)
{
	uint32_t registers[4];
	unsigned cpu_features;
	int hyperthreaded;

	__asm__ __volatile__( /* */ "cpuid ":
			     /*  */ "=a"(registers[0]),
			     /*  */ "=b"(registers[1]),
			     /*  */ "=c"(registers[2]),
			     /*  */ "=d"(registers[3]):
			     /*  */ "a"(1),
			     /*  */ "c"(0));

	cpu_features = registers[3];
	hyperthreaded = cpu_features & (1 << 28);

	return hyperthreaded;
}
#endif /* Cpu_looks_like_x86 */

int main(void)
{
	int hyperthreaded;
	unsigned apparent, physical;

	apparent = apparent_processors();
	hyperthreaded = is_hyperthreaded();

	physical = hyperthreaded ? (apparent / 2) : apparent;

	printf("CPUs: %u, physical: %u, hyperthreaded: %s\n", apparent,
	       physical, hyperthreaded ? "yes" : "no");
}
