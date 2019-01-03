/* n-choose-k.c
   Copyright (C) 2019 Eric Herman <eric@freesa.org>

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

#ifdef __GNUC__
#define umull_overflow(a, b, res_ptr) __builtin_umull_overflow(a, b, res_ptr)
#else
static int poor_diy_umull_overflow(unsigned long a, unsigned long b,
				   unsigned long *res)
{
	unsigned long c;

	c = a * b;

	if (a != 0 && (c / a) != b) {
		/* overflow detected */
		return 1;
	}

	*res = c;
	return 0;
}

#define umull_overflow(a, b, res_ptr) poor_diy_umull_overflow(a, b, res_ptr)
#endif

unsigned long n_choose_k(unsigned long n, unsigned long k, int *error)
{
	unsigned long i, result, temp1, temp2;
	int my_errno;

	if (!error) {
		error = &my_errno;
	}
	*error = 0;

	if (k > n) {
		return 0;
	}
	if (k == 0 || k == n) {
		return 1;
	}
	if (k == 1) {
		return n;
	}

	result = 1;
	for (i = 1; !*error && i <= k; ++i) {
		temp1 = (n - (k - i));
		if (umull_overflow(result, temp1, &temp2)) {
			*error = 1;
			result = 0;
		} else {
			result = temp2 / i;
		}
	}

	return result;
}

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef EOVERFLOW
#define EOVERFLOW 1
#endif

int main(int argc, char **argv)
{
	unsigned long n, k, r;
	int error;

	n = argc > 1 ? strtoul(argv[1], NULL, 10) : 8;
	k = argc > 2 ? strtoul(argv[2], NULL, 10) : 3;

	error = 0;
	r = n_choose_k(n, k, &error);
	if (error) {
		fprintf(stderr, "%lu choose %lu: factorials overflow\n", n, k);
	} else {
		printf("n_choose_k(%lu, %lu) = %lu\n", n, k, r);
	}
	return error ? EOVERFLOW : 0;
}
