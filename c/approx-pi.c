#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double iterate_approx_pi(size_t iterations, int verbose)
{
	size_t i;
	unsigned long numerator;
	unsigned long denominator;
	double pi, piover2;

	numerator = 2;
	denominator = 1;
	piover2 = 1.0;
	i = 0;

	while (i < iterations) {
		piover2 *= (((double)numerator) / ((double)denominator));
		pi = (piover2 * 2.0);
		++i;

		if (verbose) {
			printf("%6lu\t%f\t%6ld\t%6ld\n", (unsigned long)i, pi,
			       numerator, denominator);
		}

		if (1 == (i % 2)) {
			denominator += 2;
		} else {
			numerator += 2;
		}
	}

	return pi;
}

int main(int argc, const char **argv)
{
	size_t i, iterations;
	int verbose;
	unsigned base;
	char *endptr;
	double pi;

	iterations = (argc > 1) ? ((size_t)atoi(argv[1])) : 10 * 1000 * 1000;
	verbose = (argc > 2) ? atoi(argv[2]) : 0;
	pi = iterate_approx_pi(iterations, verbose);
	printf("after %lu iterations\n", (unsigned long)iterations);
	printf("pi is about: %21.18f\n", pi);
	printf("pi is known: %21.18f\n", M_PI);
	printf("      error: %21.18f\n", M_PI - pi);

	return 0;
}
