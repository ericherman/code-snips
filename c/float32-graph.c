/*
float32-graph.c: a float density mapping program
Copyright (C) 2017 Eric Herman

https://github.com/ericherman/code-snips/blob/master/c/float32-graph.c

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later
version.

gcc -std=gnu89 -O2 -g \
    -Wall -Wextra -Werror \
    -o float32-graph float32-graph.c \
    -lm &&
time { ./float32-graph /tmp/float32-graph.csv $STEP_SIZE ; }

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <errno.h>
#include <float.h>
#include <assert.h>
#include <getopt.h>

#ifndef EH_PRINT_NAN
#define EH_PRINT_NAN 0
#endif

#ifndef EH_PRINT_INF
#define EH_PRINT_INF 0
#endif

#ifndef EH_PRINT_SUBNORMAL
#define EH_PRINT_SUBNORMAL 1
#endif

#ifndef Eh_float32
#define Eh_float32 float
#endif

/* types */
enum eh_fp_class {
	eh_nan = 0,
	eh_inf = 1,
	eh_zero = 2,
	eh_subnorm = 3,
	eh_normal = 4,
};

/* prototypes */
static uint32_t eh_float32_to_uint32(Eh_float32 f);

/* The rest assumes 32bit little endian and radix 2 */

#define Eh_float32_le_r2_exp_max 127
#define Eh_float32_le_r2_exp_min -127
#define Eh_float32_le_r2_exp_inf_nan 128
#define Eh_float32_le_r2_sign_mask 0x80000000UL
#define Eh_float32_le_r2_rexp_mask 0x7F800000UL
#define Eh_float32_le_r2_frac_mask 0x007FFFFFUL
#define Eh_float32_le_r2_exp_shift 23U

static enum eh_fp_class eh_float32_radix_2_to_fields(Eh_float32 f,
						     uint8_t *sign,
						     int16_t *exponent,
						     uint32_t *fraction)
{
	uint32_t u32, raw_exp;

	u32 = eh_float32_to_uint32(f);

	*sign = (u32 & Eh_float32_le_r2_sign_mask) ? 1U : 0U;

	raw_exp = (u32 & Eh_float32_le_r2_rexp_mask);
	raw_exp = (raw_exp >> Eh_float32_le_r2_exp_shift);
	*exponent = (raw_exp - Eh_float32_le_r2_exp_max);

	*fraction = u32 & Eh_float32_le_r2_frac_mask;

	if (*exponent == Eh_float32_le_r2_exp_inf_nan) {
		if (*fraction) {
			/* +/- nan */
			return eh_nan;
		} else {
			/* +/- inf */
			return eh_inf;
		}
	}

	if (*fraction == 0 && *exponent == Eh_float32_le_r2_exp_min) {
		/* zero or -zero */
		return eh_zero;
	}

	if (*exponent == Eh_float32_le_r2_exp_min) {
		/* subnormal */
		return eh_subnorm;
	}

	return eh_normal;
}

static Eh_float32 int32_to_eh_float32(int32_t i)
{
	Eh_float32 f;
	memcpy(&f, &i, sizeof(Eh_float32));
	return f;
}

static uint32_t eh_float32_to_uint32(Eh_float32 f)
{
	uint32_t u;
	memcpy(&u, &f, sizeof(uint32_t));
	return u;
}

int main(int argc, char **argv)
{
	const char *path;
	FILE *out;
	int save_errno, print_type, print_val;
	int64_t l;
	uint32_t step_size;
	int32_t i;
	Eh_float32 f;
	uint8_t sign;
	int16_t exponent;
	uint32_t fraction;
	enum eh_fp_class t;
	const char *type;

	/* let us be clear that Eh_float32 really is 32 bits */
	assert((sizeof(Eh_float32) == sizeof(int32_t)));

	if (argc > 1) {
		path = argv[1];
		out = fopen(path, "w");
		if (!out) {
			save_errno = errno;
			fprintf(stderr, "could not open %s, error %d %s\n",
				path, save_errno, strerror(save_errno));
			return 1;
		}
	} else {
		out = stdout;
		path = NULL;
	}

	step_size = (argc > 2) ? (uint32_t)atoi(argv[2]) : 0;
	if (!step_size) {
		step_size = 0x000000FF;
	}

	print_type = (argc > 3) ? atoi(argv[3]) : 0;

	for (l = INT32_MIN; l <= INT32_MAX; l += step_size) {
		i = (int32_t)l;
		f = int32_to_eh_float32(i);
		t = eh_float32_radix_2_to_fields(f, &sign, &exponent,
						 &fraction);

		switch (t) {
		case eh_nan:
			print_val = EH_PRINT_NAN;
			type = "NAN";
			break;
		case eh_inf:
			print_val = EH_PRINT_INF;
			type = "INFINITE";
			break;
		case eh_zero:
			print_val = 1;
			type = "ZERO";
			break;
		case eh_subnorm:
			print_val = EH_PRINT_SUBNORMAL;
			type = "SUBNORMAL";
			break;
		case eh_normal:
			print_val = 1;
			type = "NORMAL";
			break;
		}
		if (print_val && print_type) {
			fprintf(out, "%d, %d, %s, %u, %d, %u, %.17g\n", i,
				(int)t, type, (unsigned)sign, (int)exponent,
				(unsigned)fraction, f);
		} else if (print_val) {
			fprintf(out, "%d, %d, %u, %d, %u, %.17g\n", i, (int)t,
				(unsigned)sign, (int)exponent,
				(unsigned)fraction, f);
		}
	}

	if (path && fclose(out)) {
		save_errno = errno;
		fprintf(stderr, "could not close %s, error %d %s\n", path,
			save_errno, strerror(save_errno));
		return 1;
	}

	return 0;
}
