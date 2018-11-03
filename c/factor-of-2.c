/* factor-of-2.c
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
