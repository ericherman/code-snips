/*
gcc -Wall -Wextra -Werror -o sine sine.c -lm; ./sine
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <errno.h>
#include <float.h>
#include <getopt.h>

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

	if (_Trig_check_for_known) {
		if (radians > 2 * M_PI || radians < -2 * M_PI) {
			x = fmod(radians, 2 * M_PI);
		} else {
			x = radians;
		}
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
	return sine_taylor((M_PI / 2) - radians);
}

double tangent_taylor(double radians)
{
	double x, tolerance;

	if (_Trig_check_for_known) {
		if (radians > 2 * M_PI || radians < -2 * M_PI) {
			x = fmod(radians, 2 * M_PI);
		} else {
			x = radians;
		}
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
	return sine_taylor(radians) / cosine_taylor(radians);
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

int compare_trig_functions(double d, double tolerance, int verbose,
			   int test_sine, int test_cosine, int test_tangent)
{
	int errors;
	errors = 0;

	if (test_sine) {
		errors +=
		    _compare_function(d, tolerance, verbose, sine_taylor,
				      "sine_taylor", sin, "   libc sin");
	}
	if (test_cosine) {
		errors +=
		    _compare_function(d, tolerance, verbose, cosine_taylor,
				      "cosine_taylor", cos, "     libc cos");
	}
	if (test_tangent) {
		errors +=
		    _compare_function(d, tolerance, verbose, tangent_taylor,
				      "tangent_taylor", tan, "      libc tan");
	}

	return errors;
}

struct sine_options_s {
	int help;
	int verbose;
	double test_double;
	int test_specific;
	int test_sin;
	int test_cos;
	int test_tan;
	int exhaustive_sin;
	int exhaustive_cos;
	int exhaustive_tan;
};

static void print_help(FILE *stream, const char *prog_name)
{
	fprintf(stream, "Usage: %s [option...]", prog_name);
	fprintf(stream, "Options:\n");
	fprintf(stream, "\t-h, --help\n");
	fprintf(stream, "\t-v, --verbose\n");
	fprintf(stream, "\t-d?, --double=?\t\ttest one specific value\n");
	fprintf(stream, "\t-s?, --test_sin=?\tdefaults to 1\n");
	fprintf(stream, "\t-c?, --test_cos=?\tdefaults to 1\n");
	fprintf(stream, "\t-t?, --test_tan=?\tdefaults to 0\n");
	fprintf(stream,
		"\t-e?, --exhaustive_sin=?\tdefaults to 0"
		" (takes about 30 minutes)\n");
	fprintf(stream, "\t-o?, --exhaustive_cos=?\tdefaults to 0\n");
	fprintf(stream, "\t-a?, --exhaustive_tan=?\tdefaults to 0\n");
}

static void parse_cmdline_args(struct sine_options_s *options, int argc,
			       char *argv[])
{
	int opt_char, option_index;

	/* yes, optstirng is horrible */
	const char *optstring = "hvd::s::c::t::e::o::a::";

	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"verbose", no_argument, 0, 'v'},
		{"double", optional_argument, 0, 'd'},
		{"test_sin", optional_argument, 0, 's'},
		{"test_cos", optional_argument, 0, 'c'},
		{"test_tan", optional_argument, 0, 't'},
		{"exhaustive_sin", optional_argument, 0, 'e'},
		{"exhaustive_cos", optional_argument, 0, 'o'},
		{"exhaustive_tan", optional_argument, 0, 'a'},
		{0, 0, 0, 0}
	};

	options->help = 0;
	options->verbose = 0;
	options->test_double = NAN;
	options->test_specific = 0;
	options->test_sin = 1;
	options->test_cos = 1;
	options->test_tan = 0;
	options->exhaustive_sin = 0;
	options->exhaustive_cos = 0;
	options->exhaustive_tan = 0;

	while (1) {
		option_index = 0;
		opt_char = getopt_long(argc, argv, optstring, long_options,
				       &option_index);

		/* Detect the end of the options. */
		if (opt_char == -1)
			break;

		switch (opt_char) {
		case 0:
			break;
		case 'h':	/* --help | -h */
			options->help = 1;
			break;
		case 'v':	/* --verbose | -v */
			options->verbose = 1;
			break;
		case 'd':	/* --double | -d */
			{
				if (optarg) {
					sscanf(optarg, "%lf",
					       &options->test_double);
				}
				options->test_specific = 1;
				options->verbose = 1;
				break;
			}
		case 's':	/* --test_sin | -s */
			options->test_sin = optarg ? atoi(optarg) : 1;
			break;
		case 'c':	/* --test_cos | -c */
			options->test_cos = optarg ? atoi(optarg) : 1;
			break;
		case 't':	/* --test_tan | -t */
			options->test_tan = optarg ? atoi(optarg) : 1;
			break;
		case 'e':	/* --exhaustive_sin | -e */
			options->exhaustive_sin = optarg ? atoi(optarg) : 1;
			break;
		case 'o':	/* --exhaustive_cos | -c */
			options->exhaustive_cos = optarg ? atoi(optarg) : 1;
			break;
		case 'a':	/* --exhaustive_tan | -t */
			options->exhaustive_tan = optarg ? atoi(optarg) : 1;
			break;
		}
	}
}

int main(int argc, char **argv)
{
	double d, tolerance, from, to;
	int total_errors, errors;
	uint64_t i;
	struct sine_options_s options;

	parse_cmdline_args(&options, argc, argv);

	if (options.help) {
		print_help(stdout, argv[0]);
	} else if (options.test_specific) {
		tolerance = 0.00001;
		total_errors =
		    compare_trig_functions(options.test_double, tolerance,
					   options.verbose, options.test_sin,
					   options.test_cos, options.test_tan);
	} else {
		total_errors = 0;
		tolerance = 0.0001;

		errors = 0;
		to = 720 * (M_PI / 180);
		from = -to;
		for (i = 0; d <= to; ++i) {
			d = (i * (M_PI / 180));
			errors +=
			    compare_trig_functions(-d, tolerance,
						   options.verbose,
						   options.test_sin,
						   options.test_cos,
						   options.test_tan);
			errors +=
			    compare_trig_functions(d, tolerance,
						   options.verbose,
						   options.test_sin,
						   options.test_cos,
						   options.test_tan);
		}
		printf("%d errors testing %lu values from %f to %f\n", errors,
		       2UL * (unsigned long)i, from, to);
		total_errors += errors;

		errors = 0;
		from = -1000;
		to = 1000;
		for (i = 0, d = from; d <= to; d += 1.0, ++i) {
			errors +=
			    compare_trig_functions(d, tolerance,
						   options.verbose,
						   options.test_sin,
						   options.test_cos,
						   options.test_tan);
		}
		printf("%d errors testing %lu values from %f to %f\n", errors,
		       (unsigned long)i, from, to);
		total_errors += errors;

		errors = 0;
		to = 1 + (M_PI * M_PI);
		from = -to;
		for (i = 0, d = from; d <= to; d += 0.00001, ++i) {
			errors +=
			    compare_trig_functions(d, tolerance,
						   options.verbose,
						   options.test_sin,
						   options.test_cos,
						   options.test_tan);
		}
		printf("%d errors testing %lu values from %f to %f\n", errors,
		       (unsigned long)i, from, to);
		total_errors += errors;

		if (options.exhaustive_sin || options.exhaustive_cos
		    || options.exhaustive_tan) {
			errors = 0;
			tolerance = 0.0000001;
			to = (double)nextafterf((float)(2 * M_PI), INFINITY);
			from = -to;
			d = from;
			for (i = 0; d <= to && i < 4294967295UL; ++i) {
				errors +=
				    compare_trig_functions(d, tolerance,
							   options.verbose,
							   options.
							   exhaustive_sin,
							   options.
							   exhaustive_cos,
							   options.
							   exhaustive_tan);
				d = (double)nextafterf((float)d, INFINITY);
			}
			printf("%d errors testing %lu values from %f to %f"
			       " (sine only)\n", errors, (unsigned long)i, from,
			       to);
			total_errors += errors;
		}
	}
	return total_errors ? 1 : 0;
}
