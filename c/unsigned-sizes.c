/*
on my machine:

$ gcc -std=c99 -o unsigned-sizes unsigned-sizes.c

$ arch
i686
$ ./unsigned-sizes
     sizeof(unsigned char):    1 bytes                       255 max
    sizeof(unsigned short):    2 bytes                     65535 max
      sizeof(unsigned int):    4 bytes                4294967295 max
     sizeof(unsigned long):    4 bytes                4294967295 max
sizeof(unsigned long long):    8 bytes      18446744073709551615 max
            sizeof(size_t):    4 bytes                4294967295 max

$ arch
x86_64
$ ./unsigned-sizes
     sizeof(unsigned char):    1 bytes                       255 max
    sizeof(unsigned short):    2 bytes                     65535 max
      sizeof(unsigned int):    4 bytes                4294967295 max
     sizeof(unsigned long):    8 bytes      18446744073709551615 max
sizeof(unsigned long long):    8 bytes      18446744073709551615 max
            sizeof(size_t):    8 bytes      18446744073709551615 max

*/

#include <stdio.h>

int main(void)
{
	printf("%26s:  %3lu bytes %25u max\n", "sizeof(unsigned char)",
	       (unsigned long)sizeof(unsigned char), (unsigned char)-1);

	printf("%26s:  %3lu bytes %25u max\n", "sizeof(unsigned short)",
	       (unsigned long)sizeof(unsigned short), (unsigned short)-1);

	printf("%26s:  %3lu bytes %25u max\n", "sizeof(unsigned int)",
	       (unsigned long)sizeof(unsigned int), (unsigned int)-1);

	printf("%26s:  %3lu bytes %25lu max\n", "sizeof(unsigned long)",
	       (unsigned long)sizeof(unsigned long), (unsigned long)-1);

	printf("%26s:  %3lu bytes %25llu max\n", "sizeof(unsigned long long)",
	       (unsigned long)sizeof(unsigned long long),
	       (unsigned long long)-1);

	printf("%26s:  %3lu bytes %25llu max\n", "sizeof(size_t)",
	       (unsigned long)sizeof(size_t), (unsigned long long)((size_t)-1));

	return 0;
}
