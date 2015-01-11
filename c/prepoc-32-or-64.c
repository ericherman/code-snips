#include <stdio.h>

#ifndef __WORDSIZE
#define DIFINED_WORDSIZE "not_defined"
#else
#if ( __WORDSIZE == 64 )
#define DIFINED_WORDSIZE "found_64"
#else
#if ( __WORDSIZE == 32 )
#define DIFINED_WORDSIZE "found_32"
#else
#define DIFINED_WORDSIZE "not_known"
#endif
#endif
#endif

int main(void)
{
	printf("DIFINED_WORDSIZE: %s\n", DIFINED_WORDSIZE);
	return sizeof(void *);
}
