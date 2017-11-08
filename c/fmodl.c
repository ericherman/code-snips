#define _GNU_SOURCE		/* for syscall(SYS_getrandom */
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>

#define Eh_float64 double	/* why isn't float64_t a thing? */

int fill_rand(void *buf, size_t buflen)
{
	unsigned int flags;

	flags = 0;
	return syscall(SYS_getrandom, buf, buflen, flags);
}

static uint64_t eh_float64_to_uint64(Eh_float64 d)
{
	uint64_t u64;

	assert((sizeof(Eh_float64) == sizeof(uint64_t)));

	memcpy(&u64, &d, sizeof(Eh_float64));
	return u64;
}

static double eh_round_toward_zero(double d)
{
	return (d < 0.0) ? -floor(-d) : floor(d);
}

/*
 * EPSILON is equal to the difference between 1.0 and the next representable
 * value, thus we'll use a *scaled* tolerance. It *might* make sense to try
 * to scale epsilon *down* between very small numbers near 0, but we will not
 * do that here.
 */
static int eh_about_equal(double d1, double d2)
{
	double abs_max, scaled_epsilon;

	abs_max = (fabs(d1) > fabs(d2)) ? fabs(d1) : fabs(d2);

	scaled_epsilon = (abs_max > 1.0) ? DBL_EPSILON * abs_max : DBL_EPSILON;

	return (fabs(d1 - d2) < scaled_epsilon) ? 1 : 0;
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

double eh_fmod(double x, double y)
{
	double n, r;
#ifdef DEBUG
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
	n = eh_round_toward_zero(x / y);
	r = x - (n * y);
#ifdef DEBUG
	if (isfinite(r)) {
		z = (y * n) + r;
		if (!eh_about_equal(x, z)) {
			save_errno = errno;
			fprintf(stderr,
				"FAIL roundtrip: %f = eh_fmod(%f, %f):\n"
				"((%f * %f) + %f) == %f\n"
				"(%f != %f) (err: %d, %s)\n\n",
				r, x, y, y, n, r, z, z, x, save_errno,
				strerror(save_errno));
		}
	}
#endif
	return r;
}

int main(void)
{
	double x, y, r, e;
	uint64_t ux, uy, ur, ue;
	size_t i, errors;

	errors = 0;
	x = 0.0;
	y = -0.0;

	for (i = 0; i < 10; ++i) {
		x = getrandom_double();
		ux = eh_float64_to_uint64(x);
		y = getrandom_double();
		uy = eh_float64_to_uint64(y);
		r = fmod(x, y);
		ur = eh_float64_to_uint64(r);
		e = eh_fmod(x, y);
		ue = eh_float64_to_uint64(e);
		if (!eh_about_equal(r, e)) {
			++errors;
			printf("   fmod(%f, %f)\n == (%f)\n", x, y, r);
			printf("eh_fmod(%f, %f)\n == (%f)\n", x, y, e);
			printf("   fmod(0x%016lx, 0x%016lx)\n == (0x%016lx)\n",
			       (unsigned long)ux, (unsigned long)uy,
			       (unsigned long)ur);
			printf("eh_fmod(0x%016lx, 0x%016lx)\n == (0x%016lx)\n",
			       (unsigned long)ux, (unsigned long)uy,
			       (unsigned long)ue);
			printf("\n");
		}
	}

	if (errors) {
		printf("differences from glibc: %lu of %lu\n",
		       (unsigned long)errors, (unsigned long)i);
	}
	return errors ? 1 : 0;
}
