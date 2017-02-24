#include <stdio.h>
#include <stdint.h>		/* uint32_t */
#include <assert.h>

#define OUTPUT_FREQ 4211

int main(void)
{
	uint32_t num, odd, even, result;
	unsigned char i, lsb_zeros;

	for (num = 2; num < (2 * 1000 * 1000 * 1000); num += 2) {
		lsb_zeros = 0;
		for (i = 0; i < 32; ++i) {
			if (num & (1U << i)) {
				break;
			}
			++lsb_zeros;
		}
		even = (1U << lsb_zeros);
		odd = num >> lsb_zeros;
		result = even * odd;
		assert(result == num);
		if (num % OUTPUT_FREQ == 0) {
			printf("%u == (%u * (2^%u))\n", num, odd, lsb_zeros);
		}
	}

	return 0;
}
