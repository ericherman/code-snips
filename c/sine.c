/*
gcc -Wall -Wextra -Werror -o sine sine.c -lm; ./sine
*/

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <errno.h>

#ifdef NDEBUG
#define debugf(...) ((void)(0))
#else
#define debugf(...) fprintf(stderr, __VA_ARGS__)
#endif

/* prototypes */
static double _factorial(uint64_t n);
static double _pow(double x, uint64_t p);

#ifndef _Sine_loop_limit
#define _Sine_loop_limit 25
#define _Sine_loop_limit_needs_undef
#endif

double sine_taylor(double x)
{
	double y, s, t, b, v;
	size_t n;

	if (isfinite(x) == 0) {
		return NAN;
	}

	debugf("sine_taylor(%f)\n", x);
	y = 0;
	for (n = 0; n <= _Sine_loop_limit; ++n) {
		s = (n % 2 == 0) ? 1.0 : -1.0;
		t = _pow(x, (2 * n) + 1);
		b = _factorial((2 * n) + 1);
		v = ((s * t) / b);
		y = y + v;

		debugf("\tn=%lu\n", (unsigned long)n);
		debugf("\ts=%f\n", s);
		debugf("\tt=%f\n", t);
		debugf("\tb=%f\n", b);
		debugf("\tv=%f\n\n", v);
		debugf("\ty=%f\n\n", y);
	}
	debugf("now, how to convert %f to radians?\n", y);
	return y;
}

#ifdef _Sine_loop_limit_needs_undef
#undef _Sine_loop_limit_needs_undef
#undef _Sine_loop_limit
#endif

static double _factorial(uint64_t n)
{
	double result;

	result = n;

	if (n < 2) {
		return 1;
	}

	do {
		result *= --n;
	} while (n > 1);

	return result;
}

static double _pow(double x, uint64_t p)
{
	double d;
	size_t i;
	d = 1;
	for (i = 1; i <= p; ++i) {
		d *= x;
	}
	return d;
}

/*
void cmp_sines(double d)
{
	printf("   glibc sin(%f) = %f\n sine_taylor(%f) = %f\n\n",
		d, sin(d), d, sine_taylor(d));
}
*/

int main(void)
{
	sine_taylor(50);
}
