/* byteswap64.c */

/*
Some systems which have bswap_32 do not have bswap_64
This defines a Bswap_64 which uses two bswap_32 calls
*/

#include <stdio.h>
#include <stdint.h>
#include <byteswap.h>

#define bswap_64_32x2(x) \
	((((uint64_t)bswap_32((uint32_t)x) << 32) | \
	((uint64_t)bswap_32((uint32_t)(x >> 32)))))

#if HAVE_BSWAP_64
#define Bswap_64(x) bswap_64(x)
#else
#define Bswap_64(x) bswap_64_32x2(x)
#endif

int main(int argc, char *argv[])
{
	uint64_t original, expected;
	uint64_t actual1, actual2, actual3;

	original = 0x554E49584C4f5645;
	expected = 0x45564f4c58494e55;

	printf("     original: %0llx\n", (unsigned long long) original);
	printf("     expected: %0llx\n", (unsigned long long) expected);

	actual1 = bswap_64(original);
	actual2 = bswap_64_32x2(original);

	printf("     bswap_64: %0llx\n", (unsigned long long) actual1);
	printf("bswap_64_32x2: %0llx\n", (unsigned long long) actual2);

	actual3 = Bswap_64(original);

	printf("     Bswap_64: %0llx\n", (unsigned long long) actual3);

	return actual2 == expected ? 0 : 1;
}
