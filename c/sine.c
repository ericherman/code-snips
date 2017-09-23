/*
gcc -Wall -Wextra -Werror -o sine sine.c -lm; ./sine
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <errno.h>
#include <float.h>

#ifndef TEST_COSINE
#define TEST_COSINE 1
#endif

#ifndef TEST_TANGENT
#define TEST_TANGENT 0
#endif

#ifndef EXHAUSTIVE_FLOAT_TEST
#define EXHAUSTIVE_FLOAT_TEST 0
#endif

#ifdef DEBUG
#define debugf(...) fprintf(stderr, __VA_ARGS__)
#endif

#ifndef debugf
#define debugf(...) ((void)(0))
#endif

#ifndef _Trig_Taylor_loop_limit
#define _Trig_Taylor_loop_limit 100U
#endif

#ifndef _Trig_Taylor_min_loops
#define _Trig_Taylor_min_loops 5U
#endif

#ifndef _Trig_check_for_known
#define _Trig_check_for_known 0
#endif

/* prototypes */
static double _factorial(uint64_t n);
static double _pow(double x, uint64_t p);
static int _float_approx_eq(double d1, double d2, double tolerance);

double sine_taylor(double radians)
{
	double x, y, s, t, b, v, last_y;
	size_t n;
	double tolerance;

	debugf("sine_taylor(%f)\n", radians);

	if (isnan(radians)) {
		return radians;
	}
	if (isfinite(radians) == 0) {
		return NAN;
	}

	if (radians > 2 * M_PI || radians < -2 * M_PI) {
		x = fmod(radians, 2 * M_PI);
	} else {
		x = radians;
	}

	if (_Trig_check_for_known) {
		tolerance = 0.0000001;
		if (_float_approx_eq(x, 0.0, tolerance)) {
			return 0;
		} else if (_float_approx_eq(x, M_PI / 6, tolerance)) {
			return 0.5;
		} else if (_float_approx_eq(x, M_PI_2, tolerance)) {
			return 1.0;
		} else if (_float_approx_eq(x, (5 * M_PI / 6), tolerance)) {
			return 0.5;
		} else if (_float_approx_eq(x, M_PI, tolerance)) {
			return 0;
		} else if (_float_approx_eq(x, (7 * M_PI) / 6, tolerance)) {
			return -0.5;
		} else if (_float_approx_eq(x, 3 * M_PI_2, tolerance)) {
			return -1.0;
		} else if (_float_approx_eq(x, (11 * M_PI / 6), tolerance)) {
			return -0.5;
		} else if (_float_approx_eq(x, 2 * M_PI, tolerance)) {
			return 0;
		}
	}

	last_y = -DBL_MAX;
	y = 0;
	tolerance = 0.000000001;
	for (n = 0; n <= _Trig_Taylor_loop_limit; ++n) {

		last_y = y;
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

		if (n >= _Trig_Taylor_min_loops &&
		    _float_approx_eq(last_y, y, tolerance)) {
			break;
		}
	}
	debugf("sine_taylor(%f) = %f\n", radians, y);
	return y;
}

double cosine_taylor(double radians)
{
	double x, tolerance;

	if (radians > 2 * M_PI || radians < -2 * M_PI) {
		x = fmod(radians, 2 * M_PI);
	} else {
		x = radians;
	}
	if (_Trig_check_for_known) {
		tolerance = 0.0000001;
		if (_float_approx_eq(x, 0, tolerance)) {
			return 1;
		} else if (_float_approx_eq(x, M_PI / 3, tolerance)) {
			return 0.5;
		} else if (_float_approx_eq(x, M_PI_2, tolerance)) {
			return 0.0;
		} else if (_float_approx_eq(x, (2 * M_PI) / 3, tolerance)) {
			return -0.5;
		} else if (_float_approx_eq(x, M_PI, tolerance)) {
			return -1.0;
		} else if (_float_approx_eq(x, (4 * M_PI) / 3, tolerance)) {
			return -0.5;
		} else if (_float_approx_eq(x, 3 * M_PI_2, tolerance)) {
			return 0.0;
		} else if (_float_approx_eq(x, (5 * M_PI) / 3, tolerance)) {
			return 0.5;
		} else if (_float_approx_eq(x, 2 * M_PI, tolerance)) {
			return 1;
		}
	}
	return sine_taylor((M_PI / 2) - x);
}

double tangent_taylor(double radians)
{
	double x, tolerance;

	if (radians > 2 * M_PI || radians < -2 * M_PI) {
		x = fmod(radians, 2 * M_PI);
	} else {
		x = radians;
	}
	if (_Trig_check_for_known) {
		tolerance = 0.0000001;
		if (_float_approx_eq(x, 0, tolerance)) {
			return 0.0;
		} else if (_float_approx_eq(x, M_PI / 4, tolerance)) {
			return 1.0;
		} else if (_float_approx_eq(x, M_PI_2, tolerance)) {
			return INFINITY;
		} else if (_float_approx_eq(x, (3 * M_PI) / 4, tolerance)) {
			return -1.0;
		} else if (_float_approx_eq(x, M_PI, tolerance)) {
			return 0.0;
		} else if (_float_approx_eq(x, (5 * M_PI) / 4, tolerance)) {
			return 1.0;
		} else if (_float_approx_eq(x, 3 * M_PI_2, tolerance)) {
			return INFINITY;
		} else if (_float_approx_eq(x, (7 * M_PI) / 4, tolerance)) {
			return -1;
		} else if (_float_approx_eq(x, 2 * M_PI, tolerance)) {
			return 0;
		}
	}
	return sine_taylor(x) / cosine_taylor(x);
}

double cotangent_taylor(double radians)
{
	return 1 / tangent_taylor(radians);
}

double secant_taylor(double radians)
{
	return 1 / cosine_taylor(radians);
}

double cosecant_taylor(double radians)
{
	return 1 / sine_taylor(radians);
}

static double _factorial(uint64_t n)
{
	double result;

	if (n < 2) {
		return 1.0;
	}

	result = n;
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

static int _float_approx_eq(double d1, double d2, double tolerance)
{
	double from, to, swap;

	if ((isnan(d1) && isnan(d2)) || (d1 == d2)) {
		return 1;
	}

	tolerance = (tolerance < 0.0) ? -tolerance : tolerance;

	if ((d2 >= 0.0 && d2 < 1.0) || (d2 < 0.0 && d2 > -1.0)) {
		from = d2 - tolerance;
		to = d2 + tolerance;
	} else {
		from = d2 * (1.0 + tolerance);
		to = d2 * (1.0 - tolerance);
	}

	if (from > to) {
		swap = from;
		from = to;
		to = swap;
	}

	return (d1 >= from && d1 <= to) ? 1 : 0;
}

typedef double (dfunc) (double x);

static int _compare_function(double d, double tolerance, int verbose, dfunc ft,
			     const char *tname, dfunc fg, const char *gname)
{
	double t, g;
	const char *prefix;
	int floats_differ;

	t = ft(d);
	g = fg(d);

	floats_differ = _float_approx_eq(t, g, tolerance) ? 0 : 1;

	if (floats_differ) {
		prefix = "ERROR! ";
	} else {
		prefix = "";
	}
	if (verbose || floats_differ) {
		printf("%s\n", prefix);
		printf("%s %s(%f) = %f\n", prefix, tname, d, t);
		printf("%s %s(%f) = %f\n", prefix, gname, d, g);
	}
	return floats_differ;
}

int compare_sine_functions(double d, double tolerance, int verbose)
{
	return _compare_function(d, tolerance, verbose, sine_taylor,
				 "sine_taylor", sin, "   libc sin");
}

int compare_trig_functions(double d, double tolerance, int verbose)
{
	int errors;
	errors = 0;

	errors +=
	    _compare_function(d, tolerance, verbose, sine_taylor, "sine_taylor",
			      sin, "   libc sin");

	if (TEST_COSINE) {
		errors +=
		    _compare_function(d, tolerance, verbose, cosine_taylor,
				      "cosine_taylor", cos, "     libc cos");
	}
	if (TEST_TANGENT) {
		errors +=
		    _compare_function(d, tolerance, verbose, tangent_taylor,
				      "tangent_taylor", tan, "      libc tan");
	}

	return errors;
}

int main(int argc, char **argv)
{
	double d, tolerance, from, to;
	int verbose, total_errors, errors;
	uint64_t i;

	if (argc > 1) {
		tolerance = 0.00001;
		sscanf(argv[1], "%lf", &d);
		verbose = 1;
		total_errors = compare_sine_functions(d, tolerance, verbose);
	} else {
		total_errors = 0;
		verbose = 0;

		tolerance = 0.0001;

		errors = 0;
		to = 720 * (M_PI / 180);
		from = -to;
		for (i = 0; d <= to; ++i) {
			d = (i * (M_PI / 180));
			errors +=
			    compare_trig_functions(-d, tolerance, verbose);
			errors += compare_trig_functions(d, tolerance, verbose);
		}
		printf("%d errors testing %lu values from %f to %f\n", errors,
		       2UL * (unsigned long)i, from, to);
		total_errors += errors;

		errors = 0;
		from = -1000;
		to = 1000;
		for (i = 0, d = from; d <= to; d += 1.0, ++i) {
			errors += compare_trig_functions(d, tolerance, verbose);
		}
		printf("%d errors testing %lu values from %f to %f\n", errors,
		       (unsigned long)i, from, to);
		total_errors += errors;

		errors = 0;
		to = 1 + (M_PI * M_PI);
		from = -to;
		for (i = 0, d = from; d <= to; d += 0.00001, ++i) {
			errors += compare_trig_functions(d, tolerance, verbose);
		}
		printf("%d errors testing %lu values from %f to %f\n", errors,
		       (unsigned long)i, from, to);
		total_errors += errors;

		if (EXHAUSTIVE_FLOAT_TEST) {
			errors = 0;
			tolerance = 0.0000001;
			to = (double)nextafterf((float)(2 * M_PI), INFINITY);
			from = -to;
			d = from;
			for (i = 0; d <= to && i < 4294967295UL; ++i) {
				errors +=
				    compare_sine_functions(d, tolerance,
							   verbose);
				d = (double)nextafterf((float)d, INFINITY);
			}
			printf
			    ("%d errors testing %lu values from %f to %f (sine only)\n",
			     errors, (unsigned long)i, from, to);
			total_errors += errors;
		}
	}
	return total_errors ? 1 : 0;
}
