/* fmodl.c
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
/*
gcc -std=gnu89 -Wall -Wextra -Wpedantic -Werror \
 -ggdb -O0 -DDEBUG=1 -o fmodl fmodl.c -lm &&
./fmodl
*/

#define _GNU_SOURCE		/* for syscall(SYS_getrandom */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>

#define Float64 double		/* why isn't float64_t a thing? */

int fill_rand(void *buf, size_t buflen)
{
	unsigned int flags;

	flags = 0;
	return syscall(SYS_getrandom, buf, buflen, flags);
}

double getrandom_double(void)
{
	double d;
	int rv, save_errno;
	unsigned char buf[sizeof(double)];

	rv = fill_rand(buf, sizeof(double));
	if (rv != sizeof(double)) {
		save_errno = errno;
		fprintf(stderr, "getrandom returned %d not %d (err: %d, %s)\n",
			rv, (int)sizeof(double), save_errno,
			strerror(save_errno));
	}
	memcpy(&d, buf, sizeof(double));
	return d;
}

static uint64_t float64_to_uint64(Float64 d)
{
	uint64_t u64;

	assert((sizeof(Float64) == sizeof(uint64_t)));

	memcpy(&u64, &d, sizeof(Float64));
	return u64;
}

static double round_toward_zero(double d)
{
	return (d < 0.0) ? -floor(-d) : floor(d);
}

/*
 * EPSILON is equal to the difference between 1.0 and the next representable
 * value, thus we'll use a *scaled* tolerance. It *might* make sense to try
 * to scale epsilon *down* between very small numbers near 0, but we will not
 * do that here.
 */
static int about_equal(double d1, double d2)
{
	double abs_max, scaled_epsilon;

	/* in this case, we'll decide all NAN are equal */
	if (isnan(d1) || isnan(d2)) {
		return (isnan(d1) && isnan(d2)) ? 1 : 0;
	}

	if (isinf(d1) && isinf(d2)) {
		return ((d1 > 0 && d2 > 0) || (d1 < 0 && d2 < 0)) ? 1 : 0;
	}

	abs_max = (fabs(d1) > fabs(d2)) ? fabs(d1) : fabs(d2);

	scaled_epsilon = (abs_max > 1.0) ? DBL_EPSILON * abs_max : DBL_EPSILON;

	return (fabs(d1 - d2) < scaled_epsilon) ? 1 : 0;
}

double our_fmod(double x, double y)
{
	double n, r;
#if DEBUG
	double z;
	int save_errno;
#endif

	/* If x or y is a NaN, a NaN is returned. */
	if (isnan(x)) {
		return x;
	}
	if (isnan(y)) {
		return y;
	}

	/* If x is an infinity, a NaN is returned. */
	/* If y is zero, a NaN is returned. */
	if (isinf(x) || y == 0.0) {
		return NAN;
	}

	/* If x is +0 (-0), and y is not zero, +0 (-0) is returned. */
	if (x == 0.0 && y != 0.0) {
		return x;
	}

	/* The return value is x - n * y, where n is the quotient of  x  /  y,
	 * rounded toward zero to an integer. */
	n = round_toward_zero(x / y);
	r = x - (n * y);
#if DEBUG
	if (isfinite(r)) {
		z = (y * n) + r;
		if (!about_equal(x, z)) {
			save_errno = errno;
			fprintf(stderr,
				"FAIL roundtrip: %g = our_fmod(%g, %g):\n"
				"((%g * %g) + %g) == %g\n"
				"(%g != %g) (err: %d, %s)\n\n",
				r, x, y, y, n, r, z, z, x, save_errno,
				strerror(save_errno));
		}
	}
#endif
	return r;
}

int main(int argc, char **argv)
{
	double x, y, libc_res, our_res;
	uint64_t ux, uy, uint_libc_res, uint_our_res;
	size_t i, limit, errors;
	int inlim, print;

	inlim = (argc > 1) ? atoi(argv[1]) : 0;
	limit = (inlim <= 0) ? 10 : (size_t)inlim;
	errors = 0;
	x = 0.0;
	y = -0.0;

	for (i = 0; i < limit; ++i) {
		do {
			x = getrandom_double();
		} while (!isnormal(x));
		ux = float64_to_uint64(x);

		do {
			y = getrandom_double();
		} while (!isnormal(y));
		uy = float64_to_uint64(y);

		libc_res = fmod(x, y);
		uint_libc_res = float64_to_uint64(libc_res);
		our_res = our_fmod(x, y);
		uint_our_res = float64_to_uint64(our_res);
		if (about_equal(libc_res, our_res)) {
#ifdef DEBUG
			print = 1;
#else
			print = 0;
#endif
		} else {
			++errors;
			print = 1;
		}
		if (print) {
			printf("    fmod(%g, %g)\n == (%g)\n", x, y, libc_res);
			printf("our_fmod(%g, %g)\n == (%g)\n", x, y, our_res);
			printf("    fmod(0x%016lx, 0x%016lx)\n == (0x%016lx)\n",
			       (unsigned long)ux, (unsigned long)uy,
			       (unsigned long)uint_libc_res);
			printf("our_fmod(0x%016lx, 0x%016lx)\n == (0x%016lx)\n",
			       (unsigned long)ux, (unsigned long)uy,
			       (unsigned long)uint_our_res);
			printf("\n");
		}
	}

	if (errors) {
		printf("differences from LibC: %lu of %lu\n",
		       (unsigned long)errors, (unsigned long)i);
	}
	return errors ? 1 : 0;
}
