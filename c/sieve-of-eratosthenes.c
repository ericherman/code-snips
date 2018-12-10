/* sieve-of-eratosthenes.c : finding primes
   Copyright (C) 2018 Eric Herman <eric@freesa.org>
   License: LGPL v2.1 or any later version */

/* gcc -g -Wall -Werror -O2 -DNDEBUG \
	-o sieve-of-eratosthenes \
	sieve-of-eratosthenes.c
*/

#include <assert.h>
#include <inttypes.h>		/* PRIu64, SCNu64 */
#include <limits.h>		/* CHAR_BIT */
#include <stdint.h>		/* uint32_t, uint64_t */
#include <stdio.h>		/* printf, sscanf */
#include <stdlib.h>		/* malloc, free */
#include <string.h>		/* memset */

struct sieve_context_s {
	uint64_t max;
	unsigned char *primes;
	size_t primes_len;
};

static void get_byte_and_offset(size_t bytes_len, uint64_t index, size_t *byte,
				unsigned char *offset)
{
	/* compiler does the right thing; no "div"s in the .s files */
	*byte = index / CHAR_BIT;
	*offset = index % CHAR_BIT;

#ifndef NDEBUG
	if ((*byte) >= bytes_len) {
		fprintf(stderr,
			"bit index %" PRIu64 " is position %zu, size is %zu\n",
			index, (*byte), bytes_len);
		assert((*byte) >= bytes_len);
	}
#endif /* NDEBUG */
}

static void set_bit(unsigned char *bytes, size_t bytes_len, uint64_t index,
		    unsigned val)
{
	size_t byte;
	unsigned char offset;

	get_byte_and_offset(bytes_len, index, &byte, &offset);

	/* This should work, but seems too tricky: */
	/* val = val ? 1 : 0 */ ;
	/* bytes[byte] ^= (-val ^ bytes[byte]) & (1U << offset); */
	/* instead, be a bit more verbose and trust the optimizer */
	if (val) {
		bytes[byte] |= (1U << offset);
	} else {
		bytes[byte] &= ~(1U << offset);
	}
}

static unsigned get_bit(unsigned char *bytes, size_t bytes_len, uint64_t index)
{
	size_t byte;
	unsigned char offset;

	get_byte_and_offset(bytes_len, index, &byte, &offset);

	return ((bytes[byte] >> offset) & 1) ? 1U : 0U;
}

static uint64_t primes_index(uint64_t index)
{
	return (index - 3) / 2;
}

static void set_not_prime(struct sieve_context_s *ctx, uint64_t number)
{
	if ((number % 2) != 0) {
		set_bit(ctx->primes, ctx->primes_len, primes_index(number), 0);
	}
}

static unsigned is_prime(struct sieve_context_s *ctx, uint64_t number)
{
	if (number < 2) {
		return 0;
	}
	if (number == 2) {
		return 1;
	}
	if ((number % 2) == 0) {
		return 0;
	}
	return get_bit(ctx->primes, ctx->primes_len, primes_index(number));
}

static void mark_non_primes(struct sieve_context_s *ctx, unsigned verbose)
{
	uint64_t i, j;

	for (i = 3; i <= ctx->max; ++i) {
		if (is_prime(ctx, i)) {
			if (verbose) {
				printf("marking for  %10" PRIu64 "\n", i);
			}
			/* mark all multiples as non-prime */
			for (j = i + i; j <= ctx->max; j += i) {
				set_not_prime(ctx, j);
			}
			if (verbose) {
				printf("done marking %10" PRIu64 "\n", i);
			}
		}
	}
	if (verbose) {
		printf("done\n");
	}
}

int main(int argc, char **argv)
{
	unsigned verbose;
	struct sieve_context_s ctx;
	uint64_t i;
	size_t size;

	verbose = 0;
	ctx.max = 0;
	ctx.primes = NULL;
	ctx.primes_len = 0;

	if (argc > 1) {
		sscanf(argv[1], "%" SCNu64 "", &(ctx.max));
	}
	if (argc > 2) {
		sscanf(argv[1], "%u", &verbose);
	}

	if (!ctx.max) {
		ctx.max = 100;
	}
	if (verbose) {
		printf("Prime numbers up to %" PRIu64 "\n", ctx.max);
	}

	/* array size is (max+1) because of zero offset */
	ctx.primes_len = (ctx.max < 3) ? 2 : primes_index(ctx.max) + 1U;
	size = ctx.primes_len * (sizeof(unsigned char));
	ctx.primes = malloc(size);
	if (!ctx.primes) {
		fprintf(stderr, "failed to malloc %zu bytes?\n", size);
		return 1;
	}
	/* and start with setting all numbers over 2 as prime */
	memset(ctx.primes, -1, ctx.primes_len);

	if (verbose) {
		printf("starting\n");
	}
	mark_non_primes(&ctx, verbose);

	for (i = 0; i <= ctx.max; ++i) {
		if (is_prime(&ctx, i)) {
			printf("%" PRIu64 "\n", i);
		}
	}

#ifndef NDEBUG
	free(ctx.primes);
#endif
	return 0;
}
