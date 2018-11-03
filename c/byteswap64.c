/* byteswap64.c
   Copyright (C) 2017, 2018 Eric Herman <eric@freesa.org>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

	https://www.gnu.org/licenses/lgpl-3.0.txt
	https://www.gnu.org/licenses/gpl-3.0.txt
 */
/* byteswap64.c */

/*
Some systems which have bswap_32 do not have bswap_64
This defines a Bswap_64 which uses two bswap_32 calls

gcc -DHAVE_BYTESWAP_H -DHAVE_BSWAP_32 -DHAVE_BSWAP_64 byteswap64.c
*/

#include <stdio.h>
#include <stdint.h>

#if HAVE_BYTESWAP_H
#include <byteswap.h>
#endif

#if HAVE_BSWAP_32
#define Bswap_32(x) bswap_32(x)
#endif

#if HAVE_BSWAP_64
#define Bswap_64 bswap_64
#endif

#define Bswap_32_constant(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

#ifndef Bswap_32
#define Bswap_32 Bswap_32_constant
#endif

#define Bswap_64_32x2(x) \
	((((uint64_t)Bswap_32((uint32_t)x) << 32) | \
	((uint64_t)Bswap_32((uint32_t)(x >> 32)))))

#ifndef Bswap_64
#define Bswap_64 Bswap_64_32x2
#endif

int main(int argc, char *argv[])
{
	uint64_t original, expected;
	uint64_t actual1, actual2, actual3;

	original = 0x554E49584C4f5645;
	expected = 0x45564f4c58494e55;

	printf("     original: %0llx\n", (unsigned long long) original);
	printf("     expected: %0llx\n", (unsigned long long) expected);

#if HAVE_BSWAP_64
	actual1 = bswap_64(original);
	printf("     bswap_64: %0llx\n", (unsigned long long) actual1);
#endif

	actual2 = Bswap_64_32x2(original);
	printf("bswap_64_32x2: %0llx\n", (unsigned long long) actual2);

	actual3 = Bswap_64(original);
	printf("     Bswap_64: %0llx\n", (unsigned long long) actual3);

	return actual2 == expected ? 0 : 1;
}
