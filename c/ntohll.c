/* ntohll.c
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
/* ntohll.c */

/*

The header arpa/inet.h declares:

       uint32_t htonl(uint32_t host32);
       uint32_t ntohl(uint32_t net32);

       uint16_t htons(uint16_t host16);
       uint16_t ntohs(uint16_t net16);

however, nothing for 64 bit ... this #defines these:

       uint64_t htonll(uint64_t host64);
       uint64_t ntohll(uint64_t net64);
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

#if HAVE_ENDIAN_H
#include <endian.h>
#define htonll(x) htobe64(x)
#define ntohll(x) be64toh(x)
#else
#define htonll(x) ((Bswap_32_constant(1) == 1) ? x : Bswap_64(x))
#define ntohll(x) ((Bswap_32_constant(1) == 1) ? x : Bswap_64(x))
#endif

int main(int argc, char *argv[])
{
	uint64_t host_le, netw_be;
	uint64_t actual1, actual2;

	if (Bswap_32_constant(1) == 1) {
		fprintf(stderr, "this expects a little endian host\n");
	}

	host_le = 0x554E49584C4f5645;
	netw_be = 0x45564f4c58494e55;

	printf("host_le: %0llx\n", (unsigned long long) host_le);
	printf("netw_be: %0llx\n", (unsigned long long) netw_be);

	actual1 = htonll(host_le);
	actual2 = ntohll(netw_be);

	printf("htonll(%0llx): %0llx\n",(unsigned long long) host_le, (unsigned long long) actual1);
	printf("ntohll(%0llx): %0llx\n",(unsigned long long) netw_be, (unsigned long long) actual2);

	return actual1 == netw_be ? 0 : 1;
}
