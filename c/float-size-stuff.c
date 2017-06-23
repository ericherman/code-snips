#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <float.h>
#include <errno.h>

int main(int argc, char **argv)
{
	int verbose;

	verbose = argc > 1 ? atoi(argv[1]) : 0;

	printf("+---------------------+-------+-------+\n");
	printf("|               type  | bytes |  bits |\n");
	printf("+---------------------+-------+-------+\n");

	if (verbose > 0) {
		printf("| %19s | %5lu | %5lu |\n",
		       "sizeof(char)", (unsigned long)sizeof(char),
		       (unsigned long)sizeof(char) * CHAR_BIT);
		printf("| %19s | %5lu | %5lu |\n",
		       "sizeof(short)", (unsigned long)sizeof(short),
		       (unsigned long)sizeof(short) * CHAR_BIT);
		printf("| %19s | %5lu | %5lu |\n",
		       "sizeof(int)", (unsigned long)sizeof(int),
		       (unsigned long)sizeof(int) * CHAR_BIT);
		printf("| %19s | %5lu | %5lu |\n",
		       "sizeof(long)", (unsigned long)sizeof(long),
		       (unsigned long)sizeof(long) * CHAR_BIT);
		printf("| %19s | %5lu | %5lu |\n",
		       "sizeof(long long)", (unsigned long)sizeof(long long),
		       (unsigned long)sizeof(long long) * CHAR_BIT);
		printf("| %19s | %5lu | %5lu |\n",
		       "sizeof(size_t)", (unsigned long)sizeof(size_t),
		       (unsigned long)sizeof(size_t) * CHAR_BIT);
		printf("| %19s | %5lu | %5lu |\n",
		       "sizeof(uint64_t)", (unsigned long)sizeof(uint64_t),
		       (unsigned long)sizeof(uint64_t) * CHAR_BIT);
	}
	printf("| %19s | %5lu | %5lu |\n",
	       "sizeof(float)", (unsigned long)sizeof(float),
	       (unsigned long)sizeof(float) * CHAR_BIT);
	printf("| %19s | %5lu | %5lu |\n",
	       "sizeof(double)", (unsigned long)sizeof(double),
	       (unsigned long)sizeof(double) * CHAR_BIT);
	printf("| %19s | %5lu | %5lu |\n",
	       "sizeof(long double)", (unsigned long)sizeof(long double),
	       (unsigned long)sizeof(long double) * CHAR_BIT);

	printf("+---------------------+-------+-------+\n");
	printf("| Constant            |         Value |\n");
	printf("+---------------------+---------------+\n");

	printf("| %19s | %13d |\n", "FLT_ROUNDS", FLT_ROUNDS);
	printf("| %19s | %13d |\n", "FLT_EVAL_METHOD", FLT_EVAL_METHOD);
	printf("| %19s | %13d |\n", "FLT_RADIX", FLT_RADIX);

	printf("| %19s | %13d |\n", "FLT_DIG", FLT_DIG);
	printf("| %19s | %13d |\n", "DBL_DIG", DBL_DIG);
	printf("| %19s | %13d |\n", "LDBL_DIG", LDBL_DIG);

	if (verbose > 1) {
		printf("| %19s | %13.11f |\n", "FLT_EPSILON", FLT_EPSILON);
		printf("| %19s | %13.11f |\n", "DBL_EPSILON", DBL_EPSILON);
		printf("| %19s | %13.11Lf |\n", "LDBL_EPSILON", LDBL_EPSILON);
	}

	printf("| %19s | %13d |\n", "FLT_MANT_DIG", FLT_MANT_DIG);
	printf("| %19s | %13d |\n", "DBL_MANT_DIG", DBL_MANT_DIG);
	printf("| %19s | %13d |\n", "LDBL_MANT_DIG", LDBL_MANT_DIG);

	printf("| %19s | %13d |\n", "FLT_MIN_EXP", FLT_MIN_EXP);
	printf("| %19s | %13d |\n", "DBL_MIN_EXP", DBL_MIN_EXP);
	printf("| %19s | %13d |\n", "LDBL_MIN_EXP", LDBL_MIN_EXP);

	printf("| %19s | %13d |\n", "FLT_MIN_10_EXP", FLT_MIN_10_EXP);
	printf("| %19s | %13d |\n", "DBL_MIN_10_EXP", DBL_MIN_10_EXP);
	printf("| %19s | %13d |\n", "LDBL_MIN_10_EXP", LDBL_MIN_10_EXP);

	printf("| %19s | %13d |\n", "FLT_MAX_EXP", FLT_MAX_EXP);
	printf("| %19s | %13d |\n", "DBL_MAX_EXP", DBL_MAX_EXP);
	printf("| %19s | %13d |\n", "LDBL_MAX_EXP", LDBL_MAX_EXP);

	printf("| %19s | %13d |\n", "FLT_MAX_10_EXP", FLT_MAX_10_EXP);
	printf("| %19s | %13d |\n", "DBL_MAX_10_EXP", DBL_MAX_10_EXP);
	printf("| %19s | %13d |\n", "LDBL_MAX_10_EXP", LDBL_MAX_10_EXP);
	printf("| %19s | %13d |\n", "DECIMAL_DIG", DECIMAL_DIG);

	if (verbose > 1) {
		printf("%20s: %f |\n", "FLT_MIN", FLT_MIN);
		printf("%20s: %f |\n", "DBL_MIN", DBL_MIN);
		printf("%20s: %Lf |\n", "LDBL_MIN", LDBL_MIN);

		printf("%20s: %f |\n", "FLT_TRUE_MIN", FLT_TRUE_MIN);
		printf("%20s: %f |\n", "DBL_TRUE_MIN", DBL_TRUE_MIN);
		printf("%20s: %Lf |\n", "LDBL_TRUE_MIN", LDBL_TRUE_MIN);

		printf("%20s: %f |\n", "FLT_MAX", FLT_MAX);
		printf("%20s: %f |\n", "DBL_MAX", DBL_MAX);
		printf("%20s: %Lf |\n", "LDBL_MAX", LDBL_MAX);
	}

	printf("+---------------------+---------------+\n");
	return 0;
}
