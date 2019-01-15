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
#include <limits.h>
#include <stdint.h>
#include <stddef.h>

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

size_t factorial_size_t(size_t n);

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
		num_perms = factorial_size_t(len - i - 1);
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
		if (!factorial_size_t(len)) {
			fprintf(stderr, "factorial(%lu) overflows\n",
				(unsigned long)len);
		}
		for (i = 0; i < factorial_size_t(len); ++i) {
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

/* in-lined from factorial_size_t.c */
/* ULS(x)? Unsigned Long Safe ?
 * if the architecture is bigger that 64 bit, fall back */
#define ULS(v) ((v <= ULONG_MAX) && (ULONG_MAX <= UINT64_MAX))

#if ((ULONG_MAX) && (ULONG_MAX <= UINT64_MAX))
const unsigned long ulfactorials[] = {
#define UL_Factorial_0 (1UL)
	UL_Factorial_0,
#define UL_Factorial_1 (1UL * UL_Factorial_0)
	UL_Factorial_1,
#define UL_Factorial_2 (2UL * UL_Factorial_1)
	UL_Factorial_2,
#define UL_Factorial_3 (3UL * UL_Factorial_2)
	UL_Factorial_3,
#define UL_Factorial_4 (4UL * UL_Factorial_3)
	UL_Factorial_4,
#define UL_Factorial_5 (5UL * UL_Factorial_4)
	UL_Factorial_5,
#define UL_Factorial_6 (6UL * UL_Factorial_5)
#if ULS(UL_Factorial_6)
	UL_Factorial_6,
#endif
#define UL_Factorial_7 (7UL * UL_Factorial_6)
#if ULS(UL_Factorial_7)
	UL_Factorial_7,
#endif
#define UL_Factorial_8 (8UL * UL_Factorial_7)
#if ULS(UL_Factorial_8)
	UL_Factorial_8,
#endif
#define UL_Factorial_9 (9UL * UL_Factorial_8)
#if ULS(UL_Factorial_9)
	UL_Factorial_9,
#endif
#define UL_Factorial_10 (10UL * UL_Factorial_9)
#if ULS(UL_Factorial_10)
	UL_Factorial_10,
#endif
#define UL_Factorial_11 (11UL * UL_Factorial_10)
#if ULS(UL_Factorial_11)
	UL_Factorial_11,
#endif
#define UL_Factorial_12 (12UL * UL_Factorial_11)
#if ULS(UL_Factorial_12)
	UL_Factorial_12,
#endif
#define UL_Factorial_13 (13UL * UL_Factorial_12)
#if ULS(UL_Factorial_13)
	UL_Factorial_13,
#endif
#define UL_Factorial_14 (14UL * UL_Factorial_13)
#if ULS(UL_Factorial_14)
	UL_Factorial_14,
#endif
#define UL_Factorial_15 (15UL * UL_Factorial_14)
#if ULS(UL_Factorial_15)
	UL_Factorial_15,
#endif
#define UL_Factorial_16 (16UL * UL_Factorial_15)
#if ULS(UL_Factorial_16)
	UL_Factorial_16,
#endif
#define UL_Factorial_17 (17UL * UL_Factorial_16)
#if ULS(UL_Factorial_17)
	UL_Factorial_17,
#endif
#define UL_Factorial_18 (18UL * UL_Factorial_17)
#if ULS(UL_Factorial_18)
	UL_Factorial_18,
#endif
#define UL_Factorial_19 (19UL * UL_Factorial_18)
#if ULS(UL_Factorial_19)
	UL_Factorial_19,
#endif
#define UL_Factorial_20 (20UL * UL_Factorial_19)
#if ULS(UL_Factorial_20)
	UL_Factorial_20,
#endif
	(0)			/* zero-terminated array for easier debugging */
};

/* minus one for zero offset, minus another for zero termination */
const size_t max_ulfactorial =
    (sizeof(ulfactorials) / sizeof(unsigned long)) - 2;
#endif

#ifndef Size_t_is_ul32
#ifdef SIZE_MAX
#ifdef UINT32_MAX
#ifdef ULONG_MAX
#define Size_t_is_ul32 ((SIZE_MAX == UINT32_MAX) && (SIZE_MAX <= ULONG_MAX))
#endif
#endif
#endif
#endif
#ifndef Size_t_is_ul32
#define Size_t_is_ul32 0
#endif

#ifndef Size_t_is_ul64
#ifdef SIZE_MAX
#ifdef UINT64_MAX
#ifdef ULONG_MAX
#define Size_t_is_ul64 ((SIZE_MAX == UINT64_MAX) && (SIZE_MAX <= ULONG_MAX))
#endif
#endif
#endif
#endif
#ifndef Size_t_is_ul64
#define Size_t_is_ul64 0
#endif

#ifndef Size_t_is_unknown
#define Size_t_is_unknown ((!(Size_t_is_ul32)) && (!(Size_t_is_ul64)))
#endif

#ifndef Max_size_t_factorial
#if Size_t_is_ul32
#define Max_size_t_factorial 12
#endif
#endif

#ifndef Max_size_t_factorial
#if Size_t_is_ul64
#define Max_size_t_factorial 20
#endif
#endif

#ifndef Max_size_t_factorial
#define Max_size_t_factorial 0
#endif

#if (!Max_size_t_factorial)

#ifndef use__builtin_umull_overflow_for_size_t
#ifdef __GNUC__
#ifdef SIZE_MAX
#ifdef ULONG_MAX
#define use__builtin_umull_overflow_for_size_t (SIZE_MAX == ULONG_MAX)
#endif
#endif
#endif
#endif
#ifndef use__builtin_umull_overflow_for_size_t
#define use__builtin_umull_overflow_for_size_t 0
#endif

#if use__builtin_umull_overflow_for_size_t
#define safe_mul_size_t(a, b, res) __builtin_umull_overflow(a, b, res)
#else
#define safe_mul_size_t(a, b, res) _diy_safe_mul_size_t(a, b, res)
static unsigned _diy_safe_mul_size_t(size_t lhs, size_t rhs, size_t *result)
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
#endif /* has__builtin_umull_overflow */

static size_t _factorial_size_t_loop(size_t n)
{
	size_t result, last_result;
	unsigned overflow;

	result = n;
	do {
		last_result = result;
		overflow = safe_mul_size_t(last_result, --n, &result);
	} while ((!overflow) && (n > 1));

	return overflow ? 0 : result;
}

#endif /* (!Max_size_t_factorial) */

size_t factorial_size_t(size_t n)
{
	size_t result;

#if Max_size_t_factorial
	result = (n > (Max_size_t_factorial)) ? 0 : ulfactorials[n];
#else
	result = (n > max_ulfactorial)
	    ? _factorial_size_t_loop(n)
	    : ulfactorials[n];
#endif

	return result;
}

#undef Max_size_t_factorial
#undef safe_mul_size_t
#undef Size_t_is_ul32
#undef Size_t_is_ul64
#undef Size_t_is_unknown
#undef use__builtin_umull_overflow_for_size_t
#undef ULS
