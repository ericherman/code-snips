/* sieve-of-eratosthenes-atomic.c : demo threads and atomics by finding primes
   Copyright (C) 2018 Eric Herman <eric@freesa.org>
   License: LGPL v2.1 or any later version */

/* gcc -g -Wall -Werror -O2 -DNDEBUG \
	-o sieve-of-eratosthenes-atomic \
	sieve-of-eratosthenes-atomic.c \
	-pthread
*/

#if MACOS
#include <sys/param.h>		/* _SC_NPROCESSORS_ONLN */
#include <sys/sysctl.h>		/* sysconf */
#else
#include <unistd.h>		/* sysconf */
#endif

#include <assert.h>
#include <inttypes.h>		/* PRIu64, SCNu64 */
#include <limits.h>		/* CHAR_BIT */
#include <pthread.h>
#include <sched.h>		/* sched_yield */
#include <stdint.h>		/* uint32_t, uint64_t */
#include <stdio.h>		/* printf, sscanf */
#include <stdlib.h>		/* malloc, free */
#include <string.h>		/* memset */

/* This list may not be complete */
#if defined __X86__ || \
	defined __i386__ || \
	defined i386 || \
	defined __386__ || \
	defined __x86_64__ || \
	defined _M_IX86 || \
	defined _M_X64
#define Cpu_looks_like_x86 1
#define is_hyperthreaded() is_hyperthreaded_x86()
#endif

#ifndef is_hyperthreaded
#define is_hyperthreaded() 0
#endif

#if Cpu_looks_like_x86
static int is_hyperthreaded_x86(void)
{
	uint32_t registers[4];
	unsigned cpu_features;
	int hyperthreaded;

	__asm__ __volatile__( /* */ "cpuid ":
			     /*  */ "=a"(registers[0]),
			     /*  */ "=b"(registers[1]),
			     /*  */ "=c"(registers[2]),
			     /*  */ "=d"(registers[3]):
			     /*  */ "a"(1),
			     /*  */ "c"(0));

	cpu_features = registers[3];
	hyperthreaded = cpu_features & (1 << 28);

	return hyperthreaded;
}
#endif /* Cpu_looks_like_x86 */

struct sieve_context_s {
	uint64_t max;
	unsigned char *primes;
	size_t primes_len;
	_Atomic uint64_t i;
};

struct mark_context_s {
	unsigned verbose;
	unsigned tid;
	struct sieve_context_s *ctx;
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
	unsigned char offset, mask;

	get_byte_and_offset(bytes_len, index, &byte, &offset);

	/* This should work, but seems too tricky: */
	/* val = val ? 1 : 0 */ ;
	/* bytes[byte] ^= (-val ^ bytes[byte]) & (1U << offset); */
	/* instead, be a bit more verbose and trust the optimizer */
	if (val) {
		mask = (1U << offset);
		/* bytes[byte] |= bit; */
		__atomic_fetch_or(bytes + byte, mask, __ATOMIC_SEQ_CST);
	} else {
		mask = ~(1U << offset);
		/* bytes[byte] &= mask; */
		__atomic_fetch_and(bytes + byte, mask, __ATOMIC_SEQ_CST);
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

static void *mark_non_primes(void *param)
{
	uint64_t i, j;
	struct mark_context_s *mctx = (struct mark_context_s *)param;
	struct sieve_context_s *ctx = mctx->ctx;

	sched_yield();

	for (i = __atomic_fetch_add(&(ctx->i), 2, __ATOMIC_SEQ_CST);
	     (i * 2) <= ctx->max;
	     i = __atomic_fetch_add(&(ctx->i), 2, __ATOMIC_SEQ_CST)) {
		if (is_prime(ctx, i)) {
			if (mctx->verbose) {
				printf("thread %3u: marking for  %10" PRIu64
				       "\n", mctx->tid, i);
			}
			/* mark all multiples as non-prime */
			for (j = i + i; j <= ctx->max; j += i) {
				set_not_prime(ctx, j);
			}
			if (mctx->verbose) {
				printf("thread %3u: done marking %10" PRIu64
				       "\n", mctx->tid, i);
			}
		}
	}
	if (mctx->verbose) {
		printf("thread %3u done\n", mctx->tid);
	}
	return NULL;
}

int main(int argc, char **argv)
{
	int hyperthreaded;
	unsigned verbose, num_processors, cores;
	struct sieve_context_s ctx;
	uint64_t i;
	size_t size;
	pthread_t *threads;
	struct mark_context_s *mctx;
	void *res;

	verbose = 0;
	ctx.max = 0;
	ctx.primes = NULL;
	ctx.primes_len = 0;
	ctx.i = 3;

	if (argc > 1) {
		sscanf(argv[1], "%" SCNu64 "", &(ctx.max));
	}
	if (argc > 2) {
		sscanf(argv[1], "%u", &verbose);
	}

	num_processors = (unsigned)sysconf(_SC_NPROCESSORS_ONLN);
	hyperthreaded = is_hyperthreaded();

	cores = hyperthreaded ? (num_processors / 2) : num_processors;

	if (verbose) {
		printf("CPUs: %u, cores: %u, hyperthreaded: %s\n",
		       num_processors, cores, hyperthreaded ? "yes" : "no");
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

	threads = calloc(cores, sizeof(pthread_t));
	if (!threads) {
		size = cores * sizeof(pthread_t);
		fprintf(stderr, "failed to malloc %zu bytes?\n", size);
		return 1;
	}

	mctx = calloc(cores, sizeof(struct mark_context_s));
	if (!threads) {
		size = cores * sizeof(struct mark_context_s);
		fprintf(stderr, "failed to malloc %zu bytes?\n", size);
		return 1;
	}
	for (i = 0; i < cores; ++i) {
		mctx[i].tid = i;
		mctx[i].verbose = verbose;
		mctx[i].ctx = &ctx;
	}

	for (i = 0; i < cores; ++i) {
		if (verbose) {
			printf("starting thread %u\n", mctx[i].tid);
		}
		pthread_create(&(threads[i]), NULL, mark_non_primes, &mctx[i]);
	}
	for (i = 0; i < cores; ++i) {
		pthread_join(threads[i], &res);
		free(res);
	}
	for (i = 0; i <= ctx.max; ++i) {
		if (is_prime(&ctx, i)) {
			printf("%" PRIu64 "\n", i);
		}
	}

#ifndef NDEBUG
	free(mctx);
	free(threads);
	free(ctx.primes);
#endif
	return 0;
}
