/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* digit_to_int: showing that masking can be used to convert ascii to int */
/* Copyright (C) 2022 Eric Herman <eric@freesa.org> */

/*
   On some CPUs, "sub" has a larger instruction latency than "and", but most
   AMD and Intel cores are the same: https://gmplib.org/~tege/x86-timing.pdf

   cc -O2 -DNDEBUG digit-to-int.c -o digit-to-int && ./digit-to-int

 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const char *ascii_digits = "0123456789";

int digit_to_int_subtract(char c)
{
	assert(c >= '0' && c <= '9');
	// x86: sub eax, 48
	return (c - '0');
}

int digit_to_int_mask(char c)
{
	assert(c >= '0' && c <= '9');
	// x86: and eax, 15
	return (c & 0x0F);
}

void validate_digit_to_int_functions(void)
{
	for (size_t i = 0; ascii_digits[i]; ++i) {
		char c = ascii_digits[i];
		int d1 = digit_to_int_subtract(c);
		int d2 = digit_to_int_mask(c);
		if (d1 != d2) {
			fprintf(stderr,
				"values do not match?\n"
				"\tdigit_to_int_subtract(%c) == %d\n"
				"\tdigit_to_int_mask(%c)     == %d\n",
				c, d1, c, d2);
			exit(EXIT_FAILURE);
		}
	}
}

clock_t time_func(int (*digit_to_int)(char), int *d)
{
	clock_t start = clock();
	for (size_t i = 0; i < CLOCKS_PER_SEC; ++i) {
		for (size_t j = 0; ascii_digits[j]; ++j) {
			char c = ascii_digits[j];
			*d += digit_to_int(c);
		}
	}
	return clock() - start;
}

int main(void)
{
	validate_digit_to_int_functions();

	int d1 = 0;
	clock_t time1 = time_func(digit_to_int_subtract, &d1);
	printf("%lld: digit_to_int_subtract (%d)\n", (long long)time1, d1);

	int d2 = 0;
	clock_t time2 = time_func(digit_to_int_mask, &d2);
	printf("%lld: digit_to_int_mask     (%d)\n", (long long)time2, d2);

	return 0;
}
