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
#include <byteswap.h>
#include <arpa/inet.h>

#define bswap_64_32x2(x) \
	((((uint64_t)bswap_32((uint32_t)x) << 32) | \
	((uint64_t)bswap_32((uint32_t)(x >> 32)))))

#if HAVE_BSWAP_64
#define Bswap_64(x) bswap_64(x)
#else
#define Bswap_64(x) bswap_64_32x2(x)
#endif

#if HAVE_HTOBE64
#include <endian.h>
#define htonll(x) htobe64(x)
#define ntohll(x) be64toh(x)
#else
#define htonll(x) ((htonl(1) == 1) ? x : Bswap_64(x))
#define ntohll(x) ((ntohl(1) == 1) ? x : Bswap_64(x))
#endif

int main(int argc, char *argv[])
{
	uint64_t host_le, netw_be;
	uint64_t actual1, actual2;

	if (htonl(1) == 1) {
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
