/*
gcc -Wall -Wextra -Werror -o sine sine.c -lm; ./sine
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#define _Trig_Taylor_loop_limit 400U
#endif

#ifndef _Trig_Taylor_min_loops
#define _Trig_Taylor_min_loops 5U
#endif

/* prototypes */
static double _factorial(uint64_t n);
static double _pow(double x, uint64_t p);
static int _double_approx_eq(double d1, double d2, double tolerance);
static double _mod_unit_circle_radians(double radians);

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

	x = _mod_unit_circle_radians(radians);

	last_y = -DBL_MAX;
	y = 0;
	/* within 100 scaled epsilon is really good enough */
	tolerance = 100.0 * DBL_EPSILON;
	for (n = 0; n <= _Trig_Taylor_loop_limit; ++n) {

		last_y = y;
		s = (n % 2 == 0) ? 1.0 : -1.0;
		t = _pow(x, (2 * n) + 1);
		b = _factorial((2 * n) + 1);
		v = ((s * t) / b);
		y = y + v;

		debugf("\tn=%llu\n", (unsigned long long)n);
		debugf("\ts=%f\n", s);
		debugf("\tt=%f\n", t);
		debugf("\tb=%f\n", b);
		debugf("\tv=%f\n\n", v);
		debugf("\ty=%f\n\n", y);

		if (n >= _Trig_Taylor_min_loops &&
		    _double_approx_eq(last_y, y, tolerance)) {
			break;
		}
	}
	debugf("sine_taylor(%f) = %f\n", radians, y);
	return y;
}

double cosine_taylor(double radians)
{
	return sine_taylor((M_PI / 2) - radians);
}

double tangent_taylor(double radians)
{
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

/* shift value to the unit circle */
/* this is, of course, a source of some error */
static double _mod_unit_circle_radians(double radians)
{
	double x;

	if (radians > 2 * M_PI || radians < -2 * M_PI) {
		x = fmod(radians, 2 * M_PI);
	} else {
		x = radians;
	}
	return x;
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

#ifndef Eh_float64
#define Eh_float64 double
#endif
uint64_t eh_float64_to_uint64(Eh_float64 d)
{
	uint64_t u64;
	memcpy(&u64, &d, sizeof(Eh_float64));
	return u64;
}

#ifndef Eh_float32
#define Eh_float32 float
#endif
uint32_t eh_float32_to_uint32(Eh_float32 d)
{
	uint32_t u32;
	memcpy(&u32, &d, sizeof(Eh_float32));
	return u32;
}

/* adapted from https://bitbashing.io/comparing-floats.html */
uint64_t _float64_distance(Eh_float64 l, Eh_float64 r)
{
	uint64_t u64a, u64b;

	if (l == r) {
		return 0;
	}

	if (isnan(l) || isnan(r) || isinf(l) || isinf(r)) {
		return UINT64_MAX;
	}

	if ((l < 0.0 && r > 0.0) || (l > 0.0 && r < 0.0)) {
		return _float64_distance(l, 0.0) + _float64_distance(r, 0.0);
	}

	u64a = eh_float64_to_uint64(l);
	u64b = eh_float64_to_uint64(r);

	return (u64a >= u64b) ? (u64a - u64b) : (u64b - u64a);
}

uint32_t _float32_distance(Eh_float32 l, Eh_float32 r)
{
	uint32_t u32a, u32b;

	if (l == r) {
		return 0;
	}

	if (isnan(l) || isnan(r) || isinf(l) || isinf(r)) {
		return UINT32_MAX;
	}

	if ((l < 0.0 && r > 0.0) || (l > 0.0 && r < 0.0)) {
		return _float32_distance(l, 0.0) + _float32_distance(r, 0.0);
	}

	u32a = eh_float32_to_uint32(l);
	u32b = eh_float32_to_uint32(r);

	return (u32a >= u32b) ? (u32a - u32b) : (u32b - u32a);
}

static double _max_d(double d1, double d2)
{
	return (d2 > d1) ? d2 : d1;
}

/*
 * EPSILON is equal to the difference between 1.0 and the next representable
 * value, thus we'll use a *scaled* tolerance. It *might* make sense to try
 * to scale epsilon *down* between very small numbers near 0, but we will not
 * do that here.
 */
static double _scale_tolerance_up(double d1, double d2, double tolerance)
{
	double abs1, abs2, abs_max;

	if (isfinite(tolerance)) {
		tolerance = fabs(tolerance);
	} else {
		tolerance = DBL_EPSILON;
	}

	abs1 = fabs(d1);
	abs2 = fabs(d2);

	abs_max = _max_d(abs1, abs2);

	if (abs_max > 1.0) {
		return tolerance * abs_max;
	}

	return tolerance;
}

/* In our case, we will assume the following:
 *
 * NAN == NAN ... in most cases, this is *REALLY* wrong,
 * in this specific case, it is what we want
 *
 */
static int _double_approx_eq(double d1, double d2, double tolerance)
{
	double diff, scaled_tolerance;

	if ((isnan(d1) && isnan(d2)) || (d1 == d2)) {
		return 1;
	}

	scaled_tolerance = _scale_tolerance_up(d1, d2, tolerance);

	diff = fabs(d1 - d2);

	return (diff <= scaled_tolerance) ? 1 : 0;
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

	floats_differ = _double_approx_eq(t, g, tolerance) ? 0 : 1;

	if (floats_differ) {
		prefix = "ERROR! ";
	} else {
		prefix = "";
	}
	if (verbose || floats_differ) {
		printf("%s\n", prefix);
		printf("%s %s(%.15f)<0x%llX>\n" "%s\t\t= %.15f <0x%llX>\n",
		       prefix, tname, d,
		       (unsigned long long)eh_float64_to_uint64(d), prefix, t,
		       (unsigned long long)eh_float64_to_uint64(t));
		printf("%s %s(%.15f)<0x%llX>\n%s\t\t= %.15f <0x%llX>\n", prefix,
		       gname, d, (unsigned long long)eh_float64_to_uint64(d),
		       prefix, g, (unsigned long long)eh_float64_to_uint64(g));
		printf("%s  float distance between %.15f\n"
		       "%s                     and %.15f\n"
		       "%s\t\t= %llu\n", prefix, t, prefix, g, prefix,
		       (unsigned long long)_float32_distance((float)t,
							     (float)g));
		printf("%s double distance between %.15f\n"
		       "%s                     and %.15f\n"
		       "%s\t\t= %llu\n", prefix, t, prefix, g, prefix,
		       (unsigned long long)_float64_distance(t, g));
		printf("\n");
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
	options->test_tan = 1;
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

int test_range(double from, double to, double inc, double tolerance,
	       int test_inv, int verbose, int test_sin, int test_cos,
	       int test_tan)
{
	int errors;
	double d;
	size_t i;

	errors = 0;
	i = 0;
	for (d = from; d <= to; d += inc) {
		errors +=
		    compare_trig_functions(d, tolerance, verbose, test_sin,
					   test_cos, test_tan);
		++i;
		if (test_inv) {
			errors +=
			    compare_trig_functions(-d, tolerance, verbose,
						   test_sin, test_cos,
						   test_tan);
			++i;
		}
	}
	printf("%d errors testing %llu values from %f t/m %f\n"
	       "\t(within %.15f)\n", errors, (unsigned long long)i,
	       (test_inv) ? -to : from, to, tolerance);
	return errors;
}

int main(int argc, char **argv)
{
	double d, tolerance, from, to, inc, test_inv;
	int total_errors, errors;
	uint64_t i;
	struct sine_options_s options;

	parse_cmdline_args(&options, argc, argv);

	if (options.help) {
		print_help(stdout, argv[0]);
	} else if (options.test_specific) {
		/* https://www.jpl.nasa.gov/edu/news/2016/3/16/how-many-decimals-of-pi-do-we-really-need/ */
		tolerance = 0.00000000000001;
		total_errors =
		    compare_trig_functions(options.test_double, tolerance,
					   options.verbose, options.test_sin,
					   options.test_cos, options.test_tan);
	} else {
		total_errors = 0;

		tolerance = 0.00000001;	/* reasonably close */
		from = 0;
		to = 1000;	/* how big is a thousand radians? */
		inc = 1;	/* big steps */
		test_inv = 1;	/* and test negative values, also */
		total_errors +=
		    test_range(from, to, inc, tolerance, test_inv,
			       options.verbose, options.test_sin,
			       options.test_cos, options.test_tan);

		tolerance = 0.000001;	/* close enough is good enough */
		to = 1 + (M_PI * M_PI);
		from = -to;
		inc = 0.00001;	/* small steps */
		total_errors +=
		    test_range(from, to, inc, tolerance, test_inv,
			       options.verbose, options.test_sin,
			       options.test_cos, options.test_tan);

		from = 0;
		inc = (M_PI / 180);	/* degree as radian */
		to = (360 * inc);	/* full 360 degrees */
		to *= 2;	/* heck, let's go twice around */
		to += 0.001;	/* and make sure we go a bit over */

		total_errors +=
		    test_range(from, to, inc, tolerance, test_inv,
			       options.verbose, options.test_sin,
			       options.test_cos, options.test_tan);

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
			printf("%d errors testing %llu values from %f to %f"
			       " (sine only)\n", errors, (unsigned long long)i,
			       from, to);
			total_errors += errors;
		}
	}
	return total_errors ? 1 : 0;
}
