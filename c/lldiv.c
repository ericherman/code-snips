#include <stdio.h>
#include <stdlib.h>		/* div, div_t */

int main(int argc, char **argv)
{
	long long numerator = (argc > 1) ? atol(argv[1]) : 5088824049625;
	long long denominator = (argc > 2) ? atol(argv[2]) : 33554393;
	lldiv_t result = lldiv(numerator, denominator);

	printf("lldiv_t result = lldiv(%llu,%llu);\n", numerator, denominator);
	printf("result.quot: %llu, result.remainder: %llu\n",
	       result.quot, result.rem);

	return 0;
}
