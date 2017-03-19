/*
on my machine:

$ gcc -std=c99 -o data-type-sizes data-type-sizes.c

$ arch
i686
$ ./data-type-sizes
                  CHAR_BIT:    8 bits
                  CHAR_MIN: -128 (signed)
     sizeof(unsigned char):    1 bytes                       255 max
    sizeof(unsigned short):    2 bytes                     65535 max
      sizeof(unsigned int):    4 bytes                4294967295 max
     sizeof(unsigned long):    4 bytes                4294967295 max
sizeof(unsigned long long):    8 bytes      18446744073709551615 max
            sizeof(size_t):    4 bytes                4294967295 max
             sizeof(float):    4 bytes
            sizeof(double):    8 bytes
       sizeof(long double):   12 bytes

$ arch
x86_64
$ ./data-type-sizes
                  CHAR_BIT:    8 bits
                  CHAR_MIN: -128 (signed)
     sizeof(unsigned char):    1 bytes                       255 max
    sizeof(unsigned short):    2 bytes                     65535 max
      sizeof(unsigned int):    4 bytes                4294967295 max
     sizeof(unsigned long):    8 bytes      18446744073709551615 max
sizeof(unsigned long long):    8 bytes      18446744073709551615 max
            sizeof(size_t):    8 bytes      18446744073709551615 max
             sizeof(float):    4 bytes
            sizeof(double):    8 bytes
       sizeof(long double):   16 bytes

*/

#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <float.h>

int main(void)
{
	printf("%26s:  %3lu bits\n", "CHAR_BIT", CHAR_BIT);
	printf("%26s: %4d (%ssigned)\n", "CHAR_MIN", (int)CHAR_MIN,
	       (CHAR_MIN == 0 ? "un" : ""));

	printf("%26s:  %3lu bytes %25lu max\n", "sizeof(unsigned char)",
	       (unsigned long)sizeof(unsigned char), (unsigned long)UCHAR_MAX);

	printf("%26s:  %3lu bytes %25lu max\n", "sizeof(unsigned short)",
	       (unsigned long)sizeof(unsigned short), (unsigned long)USHRT_MAX);

	printf("%26s:  %3lu bytes %25lu max\n", "sizeof(unsigned int)",
	       (unsigned long)sizeof(unsigned int), (unsigned long)UINT_MAX);

	printf("%26s:  %3lu bytes %25lu max\n", "sizeof(unsigned long)",
	       (unsigned long)sizeof(unsigned long), (unsigned long)ULONG_MAX);

	printf("%26s:  %3lu bytes %25llu max\n", "sizeof(unsigned long long)",
	       (unsigned long)sizeof(unsigned long long),
	       (unsigned long long)ULLONG_MAX);

	printf("%26s:  %3lu bytes %25llu max\n", "sizeof(size_t)",
	       (unsigned long)sizeof(size_t), (unsigned long long)SIZE_MAX);

	printf("%26s:  %3lu bytes\n", "sizeof(float)",
	       (unsigned long)sizeof(float));
	printf("%26s:  %3lu bytes\n", "sizeof(double)",
	       (unsigned long)sizeof(double));
	printf("%26s:  %3lu bytes\n", "sizeof(long double)",
	       (unsigned long)sizeof(long double));

	return 0;
}
