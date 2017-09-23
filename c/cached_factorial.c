/* cached__factorial.c
   Copyright (C) 2017 Eric Herman

   XXX: This is total nonsense, don't take this seriously

$ gcc -std=c89 -Wall -Wextra -Wpedantic -Werror \
    -o cached_factorial cached_factorial.c

$ ./cached__factorial 1

*/

#include <stdio.h>		/* printf */
#include <stdlib.h>		/* atoi */

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

#ifndef ULONG_MAX
#define ULONG_MAX ((unsigned long)(-1L))
#endif

unsigned long safe_mul_ul(unsigned long lhs, unsigned long rhs,
			  unsigned *overflow);


unsigned long factorial_cached(unsigned long n)
{
	unsigned long result;
	unsigned overflow;

	overflow = 0;
	result = 1;

	do {
		switch (n) {
		case 0:
		case 1:
			n = 0;
			break;
		case 2:
			result = safe_mul_ul(result, 2UL, &overflow);
			n = 0;
			break;
		case 3:
			result = safe_mul_ul(result, 6UL, &overflow);
			n = 0;
			break;
		case 4:
			result = safe_mul_ul(result, 24UL, &overflow);
			n = 0;
			break;
		case 5:
			result = safe_mul_ul(result, 120UL, &overflow);
			n = 0;
			break;
		case 6:
			result = safe_mul_ul(result, 720UL, &overflow);
			n = 0;
			break;
		case 7:
			result = safe_mul_ul(result, 5040UL, &overflow);
			n = 0;
			break;
		case 8:
			result = safe_mul_ul(result, 40320UL, &overflow);
			n = 0;
			break;
		case 9:
			result = safe_mul_ul(result, 362880UL, &overflow);
			n = 0;
			break;
		case 10:
			result = safe_mul_ul(result, 3628800UL, &overflow);
			n = 0;
			break;
		default:
			result = safe_mul_ul(result, n, &overflow);
			--n;
			break;
		}
	} while (n && !overflow);

	return overflow ? 0 : result;
}

unsigned long factorial_safe(unsigned long n)
{
	unsigned long limit, result;

	result = n;

	switch (sizeof(unsigned long)) {
	case 1: limit = 5; break;
	case 2: limit = 8; break;
	case 4: limit = 12; break;
	default:
		limit = 20;
	}

	if (n > limit) {
		return 0;
	}

	if (n < 2) {
		return 1;
	}

	do {
		result *= --n;
	} while (n > 1);

	return result;
}

unsigned long safe_mul_ul(unsigned long lhs, unsigned long rhs,
			  unsigned *overflow)
{
	const unsigned long ulong_bits = (sizeof(unsigned long) * CHAR_BIT);
	const unsigned long halfsize_max = (1ul << (ulong_bits / 2)) - 1ul;

	unsigned long lhs_hi, lhs_lo, rhs_hi, rhs_lo;
	unsigned long lowbits, midbits1, midbits2, midbits, product;

	lhs_hi = lhs >> ulong_bits / 2;
	lhs_lo = lhs & halfsize_max;
	rhs_hi = rhs >> ulong_bits / 2;
	rhs_lo = rhs & halfsize_max;

	lowbits = lhs_lo * rhs_lo;
	if (!(lhs_hi || rhs_hi)) {
		*overflow = 0;
		return lowbits;
	}
	*overflow = lhs_hi && rhs_hi;
	midbits1 = lhs_lo * rhs_hi;
	midbits2 = lhs_hi * rhs_lo;
	midbits = midbits1 + midbits2;
	*overflow = *overflow || midbits < midbits1 || midbits > halfsize_max;
	product = lowbits + (midbits << ulong_bits / 2);
	*overflow = *overflow || product < lowbits;

	return product;
}

int main(int argc, char *argv[])
{
	size_t i;
	int verbose;
	unsigned long result1, result2;
	const char *fmt;

	verbose = (argc > 1) ? atoi(argv[1]) : 0;
	fmt = "factorial_safe(%d):\t%lu\nfactorial_cached(%d):\t%lu\n";

	if(verbose) {
		printf("ULONG_MAX: %lu\n", ULONG_MAX);
	}

	for (i = 0; i < 25; ++i) {
		result1 = factorial_safe(i);
		result2 = factorial_cached(i);
		if (result1 != result2) {
			fprintf(stderr, fmt, i, result1, i, result2);
			return 1;
		}
		if (verbose) {
			printf(fmt, i, result1, i, result2);
		}
	}

	return 0;
}
