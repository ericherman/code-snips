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

#ifndef _Trig_Taylor_tolerance
	/* within scaled 1.1 x Epsilon is really good enough */
#define _Trig_Taylor_tolerance (1.1 * DBL_EPSILON)
#endif

/* sin(pi/4) : https://www.wolframalpha.com/input/?i=sin(pi%2F4) */
#define SIN_1PI_4 M_SQRT1_2
#define SIN_2PI_4 1.0
#define SIN_3PI_4 M_SQRT1_2
#define SIN_4PI_4 0.0
#define SIN_5PI_4 -M_SQRT1_2
#define SIN_3PI_2 -1.0
#define SIN_7PI_4 -M_SQRT1_2

#define COS_1PI_4 M_SQRT1_2
#define COS_2PI_4 0.0
#define COS_3PI_4 -M_SQRT1_2
#define COS_4PI_4 -1.0
#define COS_5PI_4 -M_SQRT1_2
#define COS_3PI_2 0.0
#define COS_7PI_4 M_SQRT1_2

#ifndef _Trig_tangent_use_Bernoulli
#define _Trig_tangent_use_Bernoulli 0
#endif

/* https://rosettacode.org/wiki/Bernoulli_numbers#C */
const double _Bernoulli[] = {
	1.0 / 1.0,
	-1.0 / 2.0,
	1.0 / 6.0,
	0.0,
	-1.0 / 30.0,
	0.0,
	1.0 / 42.0,
	0.0,
	-1.0 / 30.0,
	0.0,
	5.0 / 66.0,
	0.0,
	-691.0 / 2730.0,
	0.0,
	7.0 / 6.0,
	0.0,
	-3617.0 / 510.0,
	0.0,
	43867.0 / 798.0,
	0.0,
	-174611.0 / 330.0,
	0.0,
	854513.0 / 138.0,
	0.0,
	-236364091.0 / 2730.0,
	0.0,
	8553103.0 / 6.0,
	0.0,
	-23749461029.0 / 870.0,
	0.0,
	8615841276005.0 / 14322.0,
	0.0,
	-7709321041217.0 / 510.0,
	0.0,
	2577687858367.0 / 6.0,
	0.0,
	-26315271553053477373.0 / 1919190.0,
	0.0,
	2929993913841559.0 / 6.0,
	0.0,
	-261082718496449122051.0 / 13530.0,
	0.0,
	1520097643918070802691.0 / 1806.0,
	0.0,
	-27833269579301024235023.0 / 690.0,
	0.0,
	596451111593912163277961.0 / 282.0,
	0.0,
	-5609403368997817686249127547.0 / 46410.0,
	0.0,
	495057205241079648212477525.0 / 66.0,
	0.0,
	-801165718135489957347924991853.0 / 1590.0,
	0.0,
	29149963634884862421418123812691.0 / 798.0,
	0.0,
	-2479392929313226753685415739663229.0 / 870.0,
	0.0,
	84483613348880041862046775994036021.0 / 354.0,
	0.0,
	-1215233140483755572040304994079820246041491.0 / 56786730.0,
	0.0,
};

#define _Bernoulli_length (sizeof(_Bernoulli) / sizeof(double))

#define _Swap(x,y) \
	do { \
		if (&(x) != &(y)) { \
			(x) ^= (y); \
			(y) ^= (x); \
			(x) ^= (y); \
		} \
	} while (0)

/* prototypes */
double sine_taylor(double radians);
double cosine_taylor(double radians);
static double _factorial(uint64_t n);
static double _pow(double x, uint64_t p);
static int _double_approx_eq(double d1, double d2, double tolerance);
static double _mod_unit_circle_radians(double radians);

double sine_taylor(double radians)
{
#ifdef DEBUG
	static size_t _biggest_sin_taylor_loop = 0;
#endif
	double x, y, s, t, b, v, last_y;
	size_t n;
	double tolerance;

	debugf("sine_taylor(%.20f)\n", radians);

	if (isnan(radians)) {
		return radians;
	}
	if (isfinite(radians) == 0) {
		return NAN;
	}

	x = _mod_unit_circle_radians(radians);
	if (((x > (1.0 * M_PI_4)) && (x < (3.0 * M_PI_4))) ||
	    ((x > (5.0 * M_PI_4)) && (x < (7.0 * M_PI_4)))
	    ) {
		return cosine_taylor(x - M_PI_2);
	}

	/* sin(a-b) == sin(a) cos(b) - cos(a) sin(b) */

	last_y = -DBL_MAX;
	y = 0;
	tolerance = _Trig_Taylor_tolerance;
	for (n = 0; n <= _Trig_Taylor_loop_limit; ++n) {

		last_y = y;
		s = (n % 2 == 0) ? 1.0 : -1.0;
		t = _pow(x, (2 * n) + 1);
		b = _factorial((2 * n) + 1);
		v = ((s * t) / b);
		y = y + v;

		debugf("\tn=%llu\n", (unsigned long long)n);
		debugf("\ts=%.20f\n", s);
		debugf("\tt=%.20f\n", t);
		debugf("\tb=%.20f\n", b);
		debugf("\tv=%.20f\n\n", v);
		debugf("\ty=%.20f\n", y);
		debugf("\tl=(%.20f)\n", last_y);
		debugf("\n");

		if (n >= _Trig_Taylor_min_loops &&
		    _double_approx_eq(last_y, y, tolerance)) {
			break;
		}
	}
#ifdef DEBUG
	if (n > _biggest_sin_taylor_loop) {
		_biggest_sin_taylor_loop = n;
		debugf("sine_taylor(%.20f) took %llu expansions\n", radians,
		       (unsigned long long)_biggest_sin_taylor_loop);
	}
#endif
	debugf("sine_taylor(%.20f) = %.20f\n", radians, y);
	return y;
}

/* or: sin((M_PI / 2) - radians) */
double cosine_taylor(double radians)
{
#ifdef DEBUG
	static size_t _biggest_cos_taylor_loop = 0;
#endif
	double x, y, s, t, b, v, last_y;
	size_t n;
	double tolerance;

	debugf("cosine_taylor(%.20f)\n", radians);

	if (isnan(radians)) {
		return radians;
	}
	if (isfinite(radians) == 0) {
		return NAN;
	}

	x = _mod_unit_circle_radians(radians);
	if (((x > (1.0 * M_PI_4)) && (x < (3.0 * M_PI_4))) ||
	    ((x > (5.0 * M_PI_4)) && (x < (7.0 * M_PI_4)))
	    ) {
		return sine_taylor(M_PI_2 - x);
	}

	last_y = -DBL_MAX;
	y = 0;
	tolerance = _Trig_Taylor_tolerance;
	for (n = 0; n <= _Trig_Taylor_loop_limit; ++n) {

		last_y = y;
		s = (n % 2 == 0) ? 1.0 : -1.0;
		t = _pow(x, (2 * n));
		b = _factorial((2 * n));
		v = ((s * t) / b);
		y = y + v;

		debugf("\tn=%llu\n", (unsigned long long)n);
		debugf("\ts=%.20f\n", s);
		debugf("\tt=%.20f\n", t);
		debugf("\tb=%.20f\n", b);
		debugf("\tv=%.20f\n\n", v);
		debugf("\ty=%.20f\n", y);
		debugf("\tl=(%.20f)\n", last_y);
		debugf("\n");

		if (n >= _Trig_Taylor_min_loops &&
		    _double_approx_eq(last_y, y, tolerance)) {
			break;
		}
	}

#ifdef DEBUG
	if (n > _biggest_cos_taylor_loop) {
		_biggest_cos_taylor_loop = n;
		debugf("cosine_taylor(%.20f) took %llu expansions\n", radians,
		       (unsigned long long)_biggest_cos_taylor_loop);
	}
#endif
	debugf("cosine_taylor(%.20f) = %.20f\n", radians, y);
	return y;
}

/* or return sin(radians) / cos(radians) */
double tangent_taylor(double radians)
{
#if _Trig_tangent_use_Bernoulli
#ifdef DEBUG
	static size_t _biggest_tan_taylor_loop = 0;
#endif
	double x, y, s, t, b, v, last_y;
	size_t n, max_loops;
	double tolerance;

	debugf("tangent_taylor(%.20f)\n", radians);

	if (isnan(radians)) {
		return radians;
	}
	if (isfinite(radians) == 0) {
		return NAN;
	}

	x = _mod_unit_circle_radians(radians);

	last_y = -DBL_MAX;
	y = 0;
	tolerance = 0.0;
	max_loops = (_Bernoulli_length / 2);
	for (n = 1; n < max_loops; ++n) {

		last_y = y;
		t = _Bernoulli[2 * n] * _pow(-4, n) * (1 - _pow(4, n));
		b = _factorial(2 * n);
		s = _pow(x, ((2 * n) - 1));
		v = (t / b) * s;
		y = y + v;

		debugf("\tn=%llu\n", (unsigned long long)n);
		debugf("\tt=%.20f\n", t);
		debugf("\tb=%.20f\n", b);
		debugf("\ts=%.20f\n", s);
		debugf("\tv=%.20f\n\n", v);
		debugf("\ty=%.20f\n", y);
		debugf("\tl=(%.20f)\n", last_y);
		debugf("\n");

		if (n >= _Trig_Taylor_min_loops &&
		    _double_approx_eq(last_y, y, tolerance)) {
			break;
		}
	}

#ifdef DEBUG
	if (n > _biggest_tan_taylor_loop) {
		_biggest_tan_taylor_loop = n;
		debugf("tangent_taylor(%.20f) took %llu expansions\n", radians,
		       (unsigned long long)_biggest_tan_taylor_loop);
	}
#endif
	debugf("tangent_taylor(%.20f) = %.20f\n", radians, y);
	return y;
#else
	return sine_taylor(radians) / cosine_taylor(radians);
#endif
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
	long double x;

	if (radians > 2 * M_PI || radians < -2 * M_PI) {
		x = fmodl((long double)radians, 2 * M_PI);
	} else {
		x = radians;
	}
	return (double)x;
}

static double _factorial(uint64_t n)
{
	long double result;
	double small_vals[21] = {
		1.0,
		1.0,
		2.0,
		6.0,
		24.0,
		120.0,
		720.0,
		5040.0,
		40320.0,
		362880.0,
		3628800.0,
		39916800.0,
		479001600.0,
		6227020800.0,
		87178291200.0,
		1307674368000.0,
		20922789888000.0,
		355687428096000.0,
		6402373705728000.0,
		121645100408832000.0,
		2432902008176640000.0
	};

	if (n < 21) {
		return small_vals[n];
	}

	result = n;
	do {
		result *= --n;
	} while (n > 1);

	return (double)result;
}

static double _pow(double x, uint64_t p)
{
	long double d;
	size_t i;
	d = 1;
	for (i = 1; i <= p; ++i) {
		d *= x;
	}
	return (double)d;
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
	Eh_float64 tmp;

	if (l == r) {
		return 0;
	}

	if (isnan(l) || isnan(r) || isinf(l) || isinf(r)) {
		return UINT64_MAX;
	}

	if (r > l) {
		/* Sadly, we can't trust the compiler to eliminate the temp
		   variable, but it's okay https://godbolt.org/g/2ohAiB */
		tmp = l;
		l = r;
		r = tmp;
	}

	if (nextafter(r, INFINITY) == l) {
		return 1U;
	}

	/* hack around weird boundary condition around -0.0 */
	if (l > 0.0 && r < 0.0) {
		u64a = _float64_distance(l, 0.0);
		u64b = 1U + _float64_distance(r, nextafter(0.0, -INFINITY));
		return u64a + u64b;
	}

	u64a = eh_float64_to_uint64(l);
	u64b = eh_float64_to_uint64(r);
	if (u64b > u64a) {
		_Swap(u64a, u64b);
	}
	return u64a - u64b;
}

uint32_t _float32_distance(Eh_float32 l, Eh_float32 r)
{
	uint32_t u32a, u32b;
	Eh_float32 tmp;

	if (l == r) {
		return 0;
	}

	if (isnan(l) || isnan(r) || isinf(l) || isinf(r)) {
		return UINT32_MAX;
	}

	if (r > l) {
		tmp = l;
		l = r;
		r = tmp;
	}

	if (nextafter(r, INFINITY) == l) {
		return 1U;
	}

	/* hack around weird boundary condition around -0.0 */
	if (l > 0.0 && r < 0.0) {
		u32a = _float32_distance(l, 0.0);
		u32b = 1U + _float32_distance(r, nextafter(0.0, -INFINITY));
		return u32a + u32b;
	}

	u32a = eh_float32_to_uint32(l);
	u32b = eh_float32_to_uint32(r);
	if (u32b > u32a) {
		_Swap(u32a, u32b);
	}
	return u32a - u32b;
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
		printf("%s %s(%.20f)<0x%llX>\n" "%s\t\t= %.20f <0x%llX>\n",
		       prefix, tname, d,
		       (unsigned long long)eh_float64_to_uint64(d), prefix, t,
		       (unsigned long long)eh_float64_to_uint64(t));
		printf("%s %s(%.20f)<0x%llX>\n%s\t\t= %.20f <0x%llX>\n", prefix,
		       gname, d, (unsigned long long)eh_float64_to_uint64(d),
		       prefix, g, (unsigned long long)eh_float64_to_uint64(g));
		printf("%s  float distance between %.20f\n"
		       "%s                     and %.20f\n"
		       "%s                       = %llu\n", prefix, t, prefix,
		       g, prefix,
		       (unsigned long long)_float32_distance((float)t,
							     (float)g));
		printf("%s double distance between %.20f\n"
		       "%s                     and %.20f\n"
		       "%s                       = %llu\n", prefix, t, prefix,
		       g, prefix, (unsigned long long)_float64_distance(t, g));
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
	       "\t(within %.20f)\n", errors, (unsigned long long)i,
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

	total_errors = 0;

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
