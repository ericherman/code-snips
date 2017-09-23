/*
gcc -Wall -Wextra -Werror -o sine sine.c -lm; ./sine
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <errno.h>

#ifdef DEBUG
#define debugf(...) fprintf(stderr, __VA_ARGS__)
#endif

#ifndef debugf
#define debugf(...) ((void)(0))
#endif

/* prototypes */
static double _factorial(uint64_t n);
static double _pow(double x, uint64_t p);
static double _rad_to_plus_minus_pi(double x);
static int _float_approx_eq(double d1, double d2);

#ifndef _Sine_loop_limit
#define _Sine_loop_limit 25U
#endif

#ifndef _Float_eq_tolerance
#define _Float_eq_tolerance 0.000000001
#endif

double sine_taylor(double r)
{
	double x, y, s, t, b, v, last;
	size_t n;

	if (isfinite(r) == 0) {
		return NAN;
	}

	x = _rad_to_plus_minus_pi(r);

	debugf("sine_taylor(%f)\n", x);
	y = 0;
	last = -1;
	for (n = 0; n <= _Sine_loop_limit && !_float_approx_eq(y, last); ++n) {
		last = y;
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
	debugf("sine_taylor(%f) = %f\n", r, y);
	return y;
}

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

static double _rad_to_plus_minus_pi(double r)
{
	double a, x, t, m;
	uint64_t u;

	debugf("r = %f\n", r);
	if (r > M_PI) {
		t = r / M_PI;
		u = t;
		m = (u * M_PI);
		a = r - m;
		debugf("t = %f\n", t);
		debugf("u = %lu\n", (unsigned long)u);
		debugf("m = %f\n", m);
		debugf("a = %f\n", a);
		x = (u % 2 == 0) ? a : -a;
	} else if (r < -M_PI) {
		t = r / -M_PI;
		u = t;
		m = (u * M_PI);
		a = r + m;
		debugf("t = %f\n", t);
		debugf("u = %lu\n", (unsigned long)u);
		debugf("m = %f\n", m);
		debugf("a = %f\n", a);
		x = (u % 2 == 0) ? a : -a;
	} else {
		x = r;
	}
	debugf("x = %f\n", x);

	return x;
}

static int _float_approx_eq_t(double d1, double d2, double tolerance)
{
	double from, to, swap;

	if (d1 == d2) {
		return 1;
	}

	tolerance = (tolerance < 0.0) ? -tolerance : tolerance;

	from = d2 + tolerance;
	to = d2 - tolerance;

	if (from > to) {
		swap = from;
		from = to;
		to = swap;
	}

	return (d1 >= from && d1 <= to) ? 1 : 0;
}

static int _float_approx_eq(double d1, double d2)
{
	return _float_approx_eq_t(d1, d2, _Float_eq_tolerance);
}

int main(int argc, char **argv)
{
	double d, t, g, tolerance;
	const char *prefix;

	if (argc > 1) {
		sscanf(argv[1], "%lf", &d);
	} else {
		d = rand();
	}
	tolerance = 0.00001;

	t = sine_taylor(d);
	g = sin(d);

	if (_float_approx_eq_t(t, g, tolerance)) {
		prefix = "";
	} else {
		prefix = "ERROR! ";
	}
	printf("%s   libc sin(%f) = %f\n", prefix, d, g);
	printf("%ssine_taylor(%f) = %f\n", prefix, d, t);
}
