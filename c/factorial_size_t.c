/* factorial_size_t.c : pre-compute factorials for size_t values
   Copyright (c) 2019 Eric Herman <eric@freesa.org>
   License: LGPLv2.1 or at your option any later FSF version

gcc -std=c89 -Wall -Werror -Wextra -Wpedantic \
  -Wno-long-long -DTEST_FACTORIAL_SIZE_T=1 \
  factorial_size_t.c -o factorial_size_t &&
./factorial_size_t

*/

/* factorial_size_t.h: this can be extracted into a header */
#ifndef FACTORIAL_SIZE_T_H
#define FACTORIAL_SIZE_T_H
#include <limits.h>
#include <stdint.h>
#include <stddef.h>

#if ((ULONG_MAX) && (ULONG_MAX <= UINT64_MAX))
extern const unsigned long ulfactorials[];
extern const size_t max_ulfactorial;
#endif

size_t factorial_size_t(size_t n);
#endif /* FACTORIAL_SIZE_T_H */
/* END factorial_size_t.h header */

/* factorial_size_t.c */
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

#ifdef TEST_FACTORIAL_SIZE_T
#include <stdio.h>		/* printf */
#include <stdlib.h>		/* strtoul */
int main(int argc, char **argv)
{
	size_t n, result;

	n = argc > 1 ? strtoul(argv[1], NULL, 10) : 0;

	printf("SIZE_MAX: %llu\n", (unsigned long long)SIZE_MAX);
	printf("UINT32_MAX: %llu\n", (unsigned long long)UINT32_MAX);
	printf("UINT64_MAX: %llu\n", (unsigned long long)UINT64_MAX);
	printf("ULONG_MAX: %llu\n", (unsigned long long)ULONG_MAX);
#ifdef __GNUC__
	printf("__GNUC__: %d\n", __GNUC__);
#else
	printf("__GNUC__: (not defined)\n");
#endif
	printf("Max_size_t_factorial: %d\n", Max_size_t_factorial);
	printf("Size_t_is_ul32: %d\n", Size_t_is_ul32);
	printf("Size_t_is_ul64: %d\n", Size_t_is_ul64);
	printf("Size_t_is_unknown: %d\n", Size_t_is_unknown);
#ifdef use__builtin_umull_overflow_for_size_t
	printf("use__builtin_umull_overflow_for_size_t: %d\n",
	       use__builtin_umull_overflow_for_size_t);
#else
	printf("use__builtin_umull_overflow_for_size_t: (not defined)\n");
#endif
	printf("max_ulfactorial: %lu\n", (unsigned long)max_ulfactorial);

	if (n) {
		result = factorial_size_t(n);
		printf("%lu: %lu\n", (unsigned long)n, (unsigned long)result);
	} else {
		for (n = 0; n < 25; ++n) {
			result = factorial_size_t(n);
			printf("%lu: %lu\n", (unsigned long)n,
			       (unsigned long)result);
		}
	}

	return 0;
}
#endif /* TEST_FACTORIAL_SIZE_T */

#undef Max_size_t_factorial
#undef safe_mul_size_t
#undef Size_t_is_ul32
#undef Size_t_is_ul64
#undef Size_t_is_unknown
#undef use__builtin_umull_overflow_for_size_t
#undef ULS
