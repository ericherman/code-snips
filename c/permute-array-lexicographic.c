/* permute-array-lexicographic.c
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
#include <stdio.h>		/* printf */
#include <stdlib.h>		/* malloc, free */
#include <string.h>		/* memmove, memcpy */

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

/*
gcc -std=c89 -Wall -Wextra -Wpedantic -Werror \
    -o permute-array-lexicographic permute-array-lexicographic.c
# to get the 1,000,000th lexicographic ordered permutation of 10 digits:
./permute-array-lexicographic 10 999999
*/

#define MAKE_VALGRIND_HAPPY 0

size_t factorial(size_t n);
unsigned safe_mul_size_t(size_t lhs, size_t rhs, size_t *result);

/* allows caller to fetch a specific lexicographic zero-indexed permutation */
void *permute(size_t perm_idx, const void *src, void *dest, size_t len,
	      size_t elem_size, void *elem_swap_buf)
{
	size_t i, size, idx, remainder, num_perms, choice_idx;
	unsigned char *d;
	void *from, *to;

	if (!src || !dest || !len) {
		fprintf(stderr, "src: %p, dest: %p, len:%lu\n", src, dest,
			(unsigned long)len);
		return NULL;
	}

	memcpy(dest, src, len * elem_size);
	if (perm_idx == 0) {
		return dest;
	}

	idx = perm_idx;
	d = (unsigned char *)dest;	/* allow ptr math */

	for (i = 0; idx && i < len - 1; ++i) {
		num_perms = factorial(len - i - 1);
		if (!num_perms) {
			fprintf(stderr, "factorial(%lu) overflows\n",
				(unsigned long)(len - i - 1));
			return NULL;
		}
		choice_idx = idx / num_perms;
		remainder = idx % num_perms;
		to = d + (i * elem_size);
		from = d + ((i + choice_idx) * elem_size);
		if (to != from) {
			memcpy(elem_swap_buf, to, elem_size);
			memcpy(to, from, elem_size);
			if (i < (len - 2)) {
				to = d + ((i + 2) * elem_size);
				from = d + ((i + 1) * elem_size);
				size = (elem_size * (choice_idx - 1));
				memmove(to, from, size);
			}
			to = d + ((i + 1) * elem_size);
			memcpy(to, elem_swap_buf, elem_size);
		}
		idx = remainder;
	}

	return dest;
}

unsigned safe_mul_size_t(size_t lhs, size_t rhs, size_t *result)
{
	const size_t ulong_bits = (sizeof(size_t) * CHAR_BIT);
	const size_t halfsize_max = (1ul << (ulong_bits / 2)) - 1ul;

	size_t lhs_hi, lhs_lo, rhs_hi, rhs_lo;
	size_t lowbits, midbits1, midbits2, midbits, product;
	unsigned overflowed;

	lhs_hi = lhs >> ulong_bits / 2;
	lhs_lo = lhs & halfsize_max;
	rhs_hi = rhs >> ulong_bits / 2;
	rhs_lo = rhs & halfsize_max;

	lowbits = lhs_lo * rhs_lo;
	if (!(lhs_hi || rhs_hi)) {
		*result = lowbits;
		return 0;
	}
	overflowed = lhs_hi && rhs_hi;
	midbits1 = lhs_lo * rhs_hi;
	midbits2 = lhs_hi * rhs_lo;
	midbits = midbits1 + midbits2;
	overflowed = overflowed || midbits < midbits1 || midbits > halfsize_max;
	product = lowbits + (midbits << ulong_bits / 2);
	overflowed = overflowed || product < lowbits;

	*result = product;
	return overflowed;
}

size_t factorial(size_t n)
{
	size_t result, last_result;
	unsigned overflow;

	overflow = 0;
	switch (n) {
	case 0:
		result = 1UL;
		break;
	case 1:
		result = 1UL;
		break;
	case 2:
		result = 2UL;
		break;
	case 3:
		result = 6UL;
		break;
	case 4:
		result = 24UL;
		break;
	case 5:
		result = 120UL;
		break;
	case 6:
		result = 720UL;
		break;
	case 7:
		result = 5040UL;
		break;
	case 8:
		result = 40320UL;
		break;
	case 9:
		result = 362880UL;
		break;
	case 10:
		result = 3628800UL;
		break;
	case 11:
		result = 39916800UL;
		break;
	case 12:
		result = 479001600UL;
		break;
	default:
		result = n;
		do {
			last_result = result;
			overflow = safe_mul_size_t(last_result, --n, &result);
		} while ((!overflow) && (n > 1));
	}
	return overflow ? 0 : result;
}

static void print_ints(int *a, size_t len)
{
	size_t i;

	printf("[");
	for (i = 0; i < len; ++i) {
		printf("%d,", a[i]);
	}
	printf("\b]\n");
}

int main(int argc, char *argv[])
{
	int *ints, *permuted_ints, x;
	size_t i, len, elem_size;
	int permutation_number;

	len = (argc > 1) ? (size_t)atoi(argv[1]) : 4;
	permutation_number = (argc > 2) ? atoi(argv[2]) : -1;

	if (len == 0) {
		printf("[]\n");
		return 0;
	}
	elem_size = sizeof(int);
	ints = malloc(elem_size * len);
	if (!ints) {
		fprintf(stderr, "malloc(%lu) == NULL\n",
			(unsigned long)elem_size * len);
		return 1;
	}
	permuted_ints = malloc(elem_size * len);
	if (!permuted_ints) {
		fprintf(stderr, "2nd malloc(%lu) == NULL\n",
			(unsigned long)elem_size * len);
		return 1;
	}

	for (i = 0; i < len; ++i) {
		ints[i] = (int)i;
	}

	printf("---------\n");
	print_ints(ints, len);
	printf("---------\n");

	if (permutation_number < 0) {
		if (!factorial(len)) {
			fprintf(stderr, "factorial(%lu) overflows\n",
				(unsigned long)len);
		}
		for (i = 0; i < factorial(len); ++i) {
			permute(i, ints, permuted_ints, len, elem_size, &x);
			print_ints(permuted_ints, len);
		}
	} else {
		permute((size_t)permutation_number, ints, permuted_ints, len,
			elem_size, &x);
		print_ints(permuted_ints, len);
	}

	if (MAKE_VALGRIND_HAPPY) {
		free(ints);
		free(permuted_ints);
	}

	return 0;
}
