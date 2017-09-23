/*
gcc -Wall -Wextra -Werror -o sine sine.c -lm; ./sine
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <errno.h>
#include <float.h>

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

#ifndef _Sine_min_loops
#define _Sine_min_loops 5
#endif

#ifndef _Float_eq_tolerance
#define _Float_eq_tolerance 0.000000001
#endif

double sine_taylor(double radians)
{
	double x, y, s, t, b, v, last;
	size_t n;

	debugf("sine_taylor(%f)\n", radians);

	if (isnan(radians)) {
		return radians;
	}
	if (isfinite(radians) == 0) {
		return NAN;
	}

	x = _rad_to_plus_minus_pi(radians);

	last = -DBL_MAX;
	y = 0;
	for (n = 0;
	     n <= _Sine_loop_limit && !(n >= _Sine_min_loops
					&& _float_approx_eq(y, last)); ++n) {
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
	debugf("sine_taylor(%f) = %f\n", radians, y);
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
	double a, u, x, t, m;

	debugf("r = %f\n", r);
	if (r > M_PI) {
		if (r > 100000000000.0) {
			debugf("r is *very* large\n");
		}
		t = r / M_PI;
		u = floor(t);
		m = (u * M_PI);
		a = r - m;
		debugf("t = %f\n", t);
		debugf("u = %f\n", u);
		debugf("m = %f\n", m);
		debugf("a = %f\n", a);
		x = ((u / 2) == floor(u / 2)) ? a : -a;
	} else if (r < -M_PI) {
		if (r < -100000000000) {
			debugf("r is *very* large\n");
		}
		t = r / -M_PI;
		u = floor(t);
		m = (u * M_PI);
		a = r + m;
		debugf("t = %f\n", t);
		debugf("u = %f\n", u);
		debugf("m = %f\n", m);
		debugf("a = %f\n", a);
		x = ((u / 2) == floor(u / 2)) ? a : -a;
	} else {
		x = r;
	}
	debugf("x = %f\n", x);

	return x;
}

static int _float_approx_eq_t(double d1, double d2, double tolerance)
{
	double from, to, swap;

	if ((isnan(d1) && isnan(d2)) || (d1 == d2)) {
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

int compare_sine_functions(double d, double tolerance, int verbose)
{
	double t, g;
	const char *prefix;
	int floats_differ;

	t = sine_taylor(d);
	g = sin(d);

	floats_differ = !_float_approx_eq_t(t, g, tolerance);

	if (floats_differ) {
		prefix = "ERROR! ";
	} else {
		prefix = "";
	}
	if (verbose || floats_differ) {
		printf("%s   libc sin(%f) = %f\n", prefix, d, g);
		printf("%ssine_taylor(%f) = %f\n", prefix, d, t);
	}
	return floats_differ;
}

int main(int argc, char **argv)
{
	double d, tolerance, from, to;
	int verbose, errors;
	size_t i;

	if (argc > 1) {
		tolerance = 0.00001;
		sscanf(argv[1], "%lf", &d);
		verbose = 1;
		errors = compare_sine_functions(d, tolerance, verbose);
	} else {
		errors = 0;
		verbose = 0;
		tolerance = 0.0000001;

		for (i = 0; i <= 360; ++i) {
			d = i * (M_PI / 180);
			errors += compare_sine_functions(d, tolerance, verbose);
		}
		printf("%d errors testing %lu values from %f to %f\n", errors,
		       (unsigned long)i, 0.0, (360 * ((M_PI / 180))));

		from = -1000;
		to = 1000;
		for (i = 0, d = from; d <= to; d += 1.0, ++i) {
			errors += compare_sine_functions(d, tolerance, verbose);
		}
		printf("%d errors testing %lu values from %f to %f\n", errors,
		       (unsigned long)i, from, to);

		to = 1 + (M_PI * M_PI);
		from = -to;
		for (i = 0, d = from; d <= to; d += 0.000001, ++i) {
			errors += compare_sine_functions(d, tolerance, verbose);
		}
		printf("%d errors testing %lu values from %f to %f\n", errors,
		       (unsigned long)i, from, to);
	}
	return errors ? 1 : 0;
}
