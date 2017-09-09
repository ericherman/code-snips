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

static double _factorial(uint64_t n);
static double _pow(double x, uint64_t p);

#ifndef _SINE_LOOP_LIMIT
#define _SINE_LOOP_LIMIT_NEEDS_UNDEF
#define _SINE_LOOP_LIMIT 25
#endif
double sine_taylor(double x)
{
	double y, s, t, b;
	size_t n;

	if (isfinite(x) == 0) {
		return NAN;
	}

	debugf("sine_taylor(%f)\n", x);
	y = 0;
	for (n = 0; n <= _SINE_LOOP_LIMIT; ++n) {
		debugf("\tn=%lu\n", (unsigned long)n);
		s = (n % 2 == 0) ? 1.0 : -1.0;
		debugf("\ts=%f\n", s);
		t = _pow(x, (2 * n) + 1);
		debugf("\tt=%f\n", t);
		b = _factorial((2 * n) + 1);
		debugf("\tb=%f\n", b);
		y = y + ((s / b) * t);
		debugf("\ty=%f\n\n", y);
	}
	debugf("\n");
	debugf("now, how to convert to radians?\n");
	return y;
}

#ifdef _SINE_LOOP_LIMIT_NEEDS_UNDEF
#undef _SINE_LOOP_LIMIT_NEEDS_UNDEF
#undef _SINE_LOOP_LIMIT
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
