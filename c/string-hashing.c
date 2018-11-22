/* string-hashing.c: a look at various hashing functions for sharding
   Copyright (C) 2018 Eric Herman <eric@freesa.org>

   This work is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 2.1 of the
   License, or (at your option) any later version.

   This work is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License and the GNU General Public License for
   more details.

   http://www.gnu.org/licenses/
*/

/*
   gcc -O2 -o string-hashing string-hashing.c -lm &&
   cat ../COPYING |
	sed -e's/\s/\n/g' |
	sort -u |
	./string-hashing $NUM_BUCKETS $USE_JUMPHASH $VERBOSE &&
   seq 100000 | ./string-hashing $NUM_BUCKETS $USE_JUMPHASH $VERBOSE

   The purpose of this is to look at hashing from a data sharding
   perspective. Typically we use consistent hashing to reduce the
   cost of re-sharding. Numeric keys typically jumphash nicely,
   but string keys must first be converted to a numeric key for
   this to work. While this is far form a deep look, at a glance
   it seems that just about any hashing when combined with
   jumphash is fine, for the types of string keys which I am
   using. The examples above of using COPYING and `seq 100000`
   are not very representative interesting keys. Of course the
   type and length of input strings can have a very large impact
   thus if you are interested, try with your kinds of keys.
*/

#ifndef DEFAULT_NUM_BUCKETS
#define DEFAULT_NUM_BUCKETS 1024
#endif

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>

/* "This is not the best possible hash function,
   but it is short and effective."
   The C Programming Language, 2nd Edition */
unsigned int kr2_hash_str(const char *str, size_t str_len)
{
	unsigned int hash;
	size_t i;

	hash = 0;
	for (i = 0; i < str_len; ++i) {
		hash *= 31;
		hash += (unsigned int)str[i];
	}

	return hash;
}

/* "This is not the best possible algorithm,
   but it has the merit of extreme simplicity."
   The C Programming Language, 1st Edition */
unsigned int kr1_hash_str(const char *str, size_t str_len)
{
	unsigned int hash;
	size_t i;

	hash = 0;
	for (i = 0; i < str_len; ++i) {
		hash += (unsigned int)str[i];
	}

	return hash;
}

#ifndef DJB2_HASH_SEED
/*
	primes[0] == N/A
	primes[1] == 2
	primes[2] == 3
	primes[3] == 5
	primes[5] == 11
	primes[11] == 31
	primes[31] == 127
	primes[127] == 709
	primes[709] == 5381
	primes[5381] == 52711
	primes[52711] == 648391
	...
*/
#define DJB2_HASH_SEED 5381
#endif /* DJB2_HASH_SEED */
/* http://www.cse.yorku.ca/~oz/hash.html */
unsigned int djb2_hash_str(const char *str, size_t str_len)
{
	unsigned int hash;
	size_t i;

	hash = DJB2_HASH_SEED;
	for (i = 0; i < str_len; ++i) {
		hash = ((hash << 5) + hash) + ((unsigned int)str[i]);
	}

	return hash;
}

/* adapted from http://www.cse.yorku.ca/~oz/hash.html */
unsigned int djb2xor_hash_str(const char *str, size_t str_len)
{
	unsigned int hash;
	size_t i;

	hash = 5381;
	for (i = 0; i < str_len; ++i) {
		hash = (hash * 33) ^ ((unsigned int)str[i]);
	}

	return hash;
}

/* http://www.cse.yorku.ca/~oz/hash.html */
unsigned int sdbm_hash_str(const char *str, size_t str_len)
{
	unsigned int c, hash;
	size_t i;

	hash = 0;
	for (i = 0; i < str_len; ++i) {
		c = ((unsigned int)str[i]);
		hash = c + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}

/* http://www.azillionmonkeys.com/qed/hash.html */
#define get16bits(d) (*((const uint16_t *) (d)))
unsigned int paul_hsieh_super_fast_hash(const char *str, size_t str_len)
{
	unsigned int hash, tmp;
	size_t rem;

	if (str_len <= 0 || str == NULL)
		return 0;

	hash = str_len;
	rem = str_len & 3;
	str_len >>= 2;

	/* Main loop */
	for (; str_len > 0; str_len--) {
		hash += get16bits(str);
		tmp = (get16bits(str + 2) << 11) ^ hash;
		hash = (hash << 16) ^ tmp;
		str += 2 * sizeof(uint16_t);
		hash += hash >> 11;
	}

	/* Handle end cases */
	switch (rem) {
	case 3:
		hash += get16bits(str);
		hash ^= hash << 16;
		hash ^= ((signed char)str[sizeof(uint16_t)]) << 18;
		hash += hash >> 11;
		break;
	case 2:
		hash += get16bits(str);
		hash ^= hash << 11;
		hash += hash >> 17;
		break;
	case 1:
		hash += (signed char)*str;
		hash ^= hash << 10;
		hash += hash >> 1;
	}

	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}

/* https://github.com/google/leveldb/blob/master/util/coding.h#L58 */
unsigned int leveldb_decode_fixed_32(const char *ptr)
{
	return (((unsigned int)((unsigned char)(ptr[0])))
		| ((unsigned int)((unsigned char)(ptr[1])) << 8)
		| ((unsigned int)((unsigned char)(ptr[2])) << 16)
		| ((unsigned int)((unsigned char)(ptr[3])) << 24));
}

/*
  seed taken from hash_test.cc -Eric Herman
  https://github.com/google/leveldb/blob/master/util/hash_test.cc#L32
*/
const unsigned int leveldb_seed = 0xbc9f1d34;

/* https://github.com/google/leveldb/blob/master/util/hash.cc#L18 */
unsigned int leveldb_hash_str(const char *str, size_t str_len)
{
	/* Similar to murmur hash */
	const unsigned int m = 0xc6a4a793;
	const unsigned int r = 24;
	const char *limit = str + str_len;
	unsigned int h = leveldb_seed ^ (str_len * m);

	/* Pick up four bytes at a time */
	while (str + 4 <= limit) {
		unsigned int w = leveldb_decode_fixed_32(str);
		str += 4;
		h += w;
		h *= m;
		h ^= (h >> 16);
	}

	/* Pick up remaining bytes */
	switch (limit - str) {
	case 3:
		h += ((unsigned int)((unsigned char)(str[2]))) << 16;
		/* fallthrough */
	case 2:
		h += ((unsigned int)((unsigned char)(str[1]))) << 8;
		/* fallthrough */
	case 1:
		h += ((unsigned int)((unsigned char)(str[0])));
		h *= m;
		h ^= (h >> r);
		break;
	}
	return h;
}

#define ROTL64(x,r) (((uint64_t)x << r) | ((uint64_t)x >> (64 - r)))

#define U8TO64_LE(p) \
	(((uint64_t)((p)[0])) | ((uint64_t)((p)[1]) << 8) |        \
	 ((uint64_t)((p)[2]) << 16) | ((uint64_t)((p)[3]) << 24) | \
	 ((uint64_t)((p)[4]) << 32) | ((uint64_t)((p)[5]) << 40) | \
	 ((uint64_t)((p)[6]) << 48) | ((uint64_t)((p)[7]) << 56))

#define SIPROUND \
	do {				\
		v0 += v1;		\
		v1 = ROTL64(v1, 13);	\
		v1 ^= v0;		\
		v0 = ROTL64(v0, 32);	\
		v2 += v3;		\
		v3 = ROTL64(v3, 16);	\
		v3 ^= v2;		\
		v0 += v3;		\
		v3 = ROTL64(v3, 21);	\
		v3 ^= v0;		\
		v2 += v1;		\
		v1 = ROTL64(v1, 17);	\
		v1 ^= v2;		\
		v2 = ROTL64(v2, 32);	\
	} while (0)

/* adapted from https://github.com/veorq/SipHash/blob/master/siphash.c */
unsigned int sip_hash_str(const char *str, size_t str_len)
{
	/* default: SipHash-2-4 */
	uint64_t v0 = 0x736f6d6570736575ULL;
	uint64_t v1 = 0x646f72616e646f6dULL;
	uint64_t v2 = 0x6c7967656e657261ULL;
	uint64_t v3 = 0x7465646279746573ULL;
	uint64_t k0 = 0;
	uint64_t k1 = 0;
	uint64_t m;
	const char *end = str + str_len - (str_len % sizeof(uint64_t));
	const int left = str_len & 7;
	uint64_t b = ((uint64_t)str_len) << 56;
	v3 ^= k1;
	v2 ^= k0;
	v1 ^= k1;
	v0 ^= k0;

	for (; str != end; str += 8) {
		m = U8TO64_LE(str);
		v3 ^= m;
		SIPROUND;
		SIPROUND;
		v0 ^= m;
	}

	switch (left) {
	case 7:
		b |= ((uint64_t)str[6]) << 48;
		/* fallthrough */
	case 6:
		b |= ((uint64_t)str[5]) << 40;
		/* fallthrough */
	case 5:
		b |= ((uint64_t)str[4]) << 32;
		/* fallthrough */
	case 4:
		b |= ((uint64_t)str[3]) << 24;
		/* fallthrough */
	case 3:
		b |= ((uint64_t)str[2]) << 16;
		/* fallthrough */
	case 2:
		b |= ((uint64_t)str[1]) << 8;
		/* fallthrough */
	case 1:
		b |= ((uint64_t)str[0]);
		break;
	case 0:
		break;
	}

	v3 ^= b;
	SIPROUND;
	SIPROUND;
	v0 ^= b;

	v2 ^= 0xff;

	SIPROUND;
	SIPROUND;
	SIPROUND;
	SIPROUND;

	b = v0 ^ v1 ^ v2 ^ v3;
	return b;
}

/* https://en.wikipedia.org/wiki/MurmurHash */
unsigned int murmur_hash_str(const char *str, size_t len)
{
	uint32_t k;
	size_t i;
	uint32_t hash = 0;
	const uint32_t *str_x4;

	if (len > 3) {
		str_x4 = (const uint32_t *)str;
		i = len >> 2;
		do {
			uint32_t k = *str_x4++;
			k *= 0xcc9e2d51;
			k = (k << 15) | (k >> 17);
			k *= 0x1b873593;
			hash ^= k;
			hash = (hash << 13) | (hash >> 19);
			hash = (hash * 5) + 0xe6546b64;
		} while (--i);
		str = (const char *)str_x4;
	}
	if (len & 3) {
		i = len & 3;
		k = 0;
		str = &str[i - 1];
		do {
			k <<= 8;
			k |= *str--;
		} while (--i);
		k *= 0xcc9e2d51;
		k = (k << 15) | (k >> 17);
		k *= 0x1b873593;
		hash ^= k;
	}
	hash ^= len;
	hash ^= hash >> 16;
	hash *= 0x85ebca6b;
	hash ^= hash >> 13;
	hash *= 0xc2b2ae35;
	hash ^= hash >> 16;
	return hash;
}

#define Rol_size_val_pos(size_bytes, val, positions) \
	( ((val) << ((positions) % ((size_bytes) * CHAR_BIT))) \
	| ((val) >> (((size_bytes) * CHAR_BIT) \
		- ((positions) % ((size_bytes) * CHAR_BIT)))) )

#define Ror_size_val_pos(size_bytes, val, positions) \
	( ((val) >> ((positions) % ((size_bytes) * CHAR_BIT))) \
	| ((val) << (((size_bytes) * CHAR_BIT) \
		- ((positions) % ((size_bytes) * CHAR_BIT)))) )

/* dumb idea; better than kr1, but for short strings, not even as good as kr2 */
unsigned int xorrot_hash_str(const char *str, size_t str_len)
{
	unsigned int hash;
	size_t i, bits;

	hash = 0;
	for (i = 0; i < str_len; ++i) {
		hash ^= Ror_size_val_pos(sizeof(unsigned), (unsigned)str[i], i);
		hash ^= Rol_size_val_pos(sizeof(unsigned), (unsigned)str[i], i);
		hash ^= hash >> CHAR_BIT;
		hash ^= hash << CHAR_BIT;
	}

	return hash;
}

/* https://github.com/ericherman/libjumphash */
/* see also: http://random.mat.sbg.ac.at/results/karl/server/node5.html */
#define Linear_Congruential_Generator_64 2862933555777941757ULL
/* essentially a copy-paste from  https://arxiv.org/pdf/1406.2294v1.pdf */
int32_t jumphash(uint64_t key, int32_t num_buckets)
{
	int64_t b = -1;
	int64_t j = 0;

	while (j < num_buckets) {
		b = j;
		key = key * Linear_Congruential_Generator_64 + 1;
		j = (b + 1) * ((double)(1LL << 31) / (double)((key >> 33) + 1));
	}
	return ((int32_t)b);
}

/* https://github.com/ericherman/simple_stats/ */
struct simple_stats_s {
	unsigned int cnt;
	double min;
	double max;
	double sum;
	double sum_of_squares;
};

void simple_stats_init(struct simple_stats_s *stats)
{
	stats->cnt = 0;
	stats->min = DBL_MAX;
	stats->max = -DBL_MAX;
	stats->sum = 0.0;
	stats->sum_of_squares = 0.0;
}

void simple_stats_append_val(struct simple_stats_s *stats, double val)
{
	stats->cnt++;
	if (stats->min > val) {
		stats->min = val;
	}
	if (stats->max < val) {
		stats->max = val;
	}
	stats->sum += val;
	stats->sum_of_squares += (val * val);
}

double simple_stats_average(struct simple_stats_s *stats)
{
	return stats->sum / stats->cnt;
}

double simple_stats_variance(struct simple_stats_s *stats, int bessel_correct)
{
	double avg_sum_squared, avg_diff_sum_sq, variance;
	size_t bassel_cnt;

	/*   avoid division by zero */
	if (stats->cnt == 0) {
		return NAN;
	}
	if (stats->cnt == 1) {
		return 0.0;
	}

	/* https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance */

	/*  Because SumSq and (Sum * Sum)/n can be very similar
	 *  numbers, cancellation can lead to the precision of the result to
	 *  be much less than the inherent precision of the floating-point
	 *  arithmetic used to perform the computation. Thus this algorithm
	 *  should not be used str practice. */

	bassel_cnt = bessel_correct ? (stats->cnt - 1) : stats->cnt;
	avg_sum_squared = (stats->sum * stats->sum) / stats->cnt;
	avg_diff_sum_sq = stats->sum_of_squares - avg_sum_squared;
	variance = avg_diff_sum_sq / bassel_cnt;
	return fabs(variance);
}

double simple_stats_std_dev(struct simple_stats_s *stats, int bessel_correct)
{
	return sqrt(simple_stats_variance(stats, bessel_correct));
}

static int _print_stats(struct simple_stats_s *stats, const char *name,
			double t_val, int bessel_correct)
{
	return printf("%s\t%u\t%u\t%g\t%g\t%g\n", name, (unsigned)stats->min,
		      (unsigned)stats->max, simple_stats_average(stats),
		      simple_stats_std_dev(stats, bessel_correct), t_val);
}

static double _time_as_double()
{
	struct timespec t;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);
	return ((double)t.tv_sec) + (((double)t.tv_nsec) * 1e-9);
}

static size_t _bucket_for(unsigned int hash_str, size_t num_buckets,
			  int force_consistent_hashing)
{
	size_t bucket;

	if (force_consistent_hashing) {
		bucket = (size_t)jumphash(hash_str, num_buckets);
	} else {
		bucket = hash_str % num_buckets;
	}

	return bucket;
}

#define MAX_WORD_LEN 80
#define WORD_SCANF_FMT "%79s"

#define Hash_it(FUNC, TIME, BUCKET) \
	do { \
		t0 = _time_as_double(); \
		hashcode = FUNC(buf, str_len); \
		TIME += (_time_as_double() - t0); \
		bucket = _bucket_for(hashcode, num_buckets, \
				     force_consistent_hashing); \
		++(BUCKET[bucket]); \
	} while (0)

int main(int argc, char **argv)
{
	size_t i, inputs, str_len, num_buckets, bucket;
	int force_consistent_hashing, verbose, bessel_correct;
	char buf[MAX_WORD_LEN];
	unsigned int hashcode;
	unsigned int *kr2, *kr1, *djb2, *djb2xor, *sdbm, *paul, *level, *sip,
	    *murmur, *xorrot;
	double t0, t_kr2, t_kr1, t_djb2, t_djb2xor, t_sdbm, t_paul, t_level,
	    t_sip, t_murmur, t_xorrot;
	struct simple_stats_s ss_kr2, ss_kr1, ss_djb2, ss_djb2xor, ss_sdbm,
	    ss_paul, ss_level, ss_sip, ss_murmur, ss_xorrot;

	num_buckets = (argc > 1) ? strtoul(argv[1], NULL, 10) : 0;
	force_consistent_hashing = (argc > 2) ? atoi(argv[2]) : 1;
	verbose = (argc > 3) ? atoi(argv[3]) : 0;
	bessel_correct = (argc > 4) ? atoi(argv[4]) : 1;

	if (num_buckets < 1) {
		num_buckets = DEFAULT_NUM_BUCKETS;
	}

	t_kr2 = t_kr1 = t_djb2 = t_djb2xor = t_sdbm = t_paul = t_level = 0;
	t_sip = t_murmur = t_xorrot = 0;

	simple_stats_init(&ss_kr2);
	simple_stats_init(&ss_kr1);
	simple_stats_init(&ss_djb2);
	simple_stats_init(&ss_djb2xor);
	simple_stats_init(&ss_sdbm);
	simple_stats_init(&ss_paul);
	simple_stats_init(&ss_level);
	simple_stats_init(&ss_sip);
	simple_stats_init(&ss_murmur);
	simple_stats_init(&ss_xorrot);

	kr2 = calloc(sizeof(unsigned int), num_buckets);
	kr1 = calloc(sizeof(unsigned int), num_buckets);
	djb2 = calloc(sizeof(unsigned int), num_buckets);
	djb2xor = calloc(sizeof(unsigned int), num_buckets);
	sdbm = calloc(sizeof(unsigned int), num_buckets);
	paul = calloc(sizeof(unsigned int), num_buckets);
	level = calloc(sizeof(unsigned int), num_buckets);
	sip = calloc(sizeof(unsigned int), num_buckets);
	murmur = calloc(sizeof(unsigned int), num_buckets);
	xorrot = calloc(sizeof(unsigned int), num_buckets);

	if (!kr2 || !kr1 || !djb2 || !djb2xor || !sdbm || !paul || !level
	    || !sip || !murmur || !xorrot) {
		fprintf(stderr, "failed to calloc(%lu, %lu)?\n",
			(unsigned long)(sizeof(unsigned int)),
			(unsigned long)num_buckets);
		return 1;
	}

	inputs = 0;
	while (!feof(stdin)) {
		if (!fscanf(stdin, WORD_SCANF_FMT, buf)) {
			continue;
		}
		str_len = strnlen(buf, MAX_WORD_LEN);
		++inputs;

		Hash_it(kr2_hash_str, t_kr2, kr2);
		Hash_it(kr1_hash_str, t_kr1, kr1);
		Hash_it(djb2_hash_str, t_djb2, djb2);
		Hash_it(djb2xor_hash_str, t_djb2xor, djb2xor);
		Hash_it(sdbm_hash_str, t_sdbm, sdbm);
		Hash_it(paul_hsieh_super_fast_hash, t_paul, paul);
		Hash_it(leveldb_hash_str, t_level, level);
		Hash_it(sip_hash_str, t_sip, sip);
		Hash_it(murmur_hash_str, t_murmur, murmur);
		Hash_it(xorrot_hash_str, t_xorrot, xorrot);
	}

	if (verbose) {
		printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
		       "kr2", "kr1", "djb2", "djb2xor", "sdbm", "paul",
		       "leveldb", "sip", "murmur", "xorrot");
	}
	for (i = 0; i < num_buckets; ++i) {
		simple_stats_append_val(&ss_kr2, kr2[i]);
		simple_stats_append_val(&ss_kr1, kr1[i]);
		simple_stats_append_val(&ss_djb2, djb2[i]);
		simple_stats_append_val(&ss_djb2xor, djb2xor[i]);
		simple_stats_append_val(&ss_sdbm, sdbm[i]);
		simple_stats_append_val(&ss_paul, paul[i]);
		simple_stats_append_val(&ss_level, level[i]);
		simple_stats_append_val(&ss_sip, sip[i]);
		simple_stats_append_val(&ss_murmur, murmur[i]);
		simple_stats_append_val(&ss_xorrot, xorrot[i]);
		if (verbose) {
			printf("%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\n",
			       kr2[i], kr1[i], djb2[i], djb2xor[i], sdbm[i],
			       paul[i], level[i], sip[i], murmur[i], xorrot[i]);
		}
	}
	if (verbose) {
		printf("\n");
	}
	printf("%lu values in %lu buckets\n", (unsigned long)inputs,
	       (unsigned long)num_buckets);
	printf("bucket chosen using %s\n",
	       force_consistent_hashing ? "jumphash" : "mod");

	printf("\tmin\tmax\tavg\tstd-dev\ttime\n");
	_print_stats(&ss_kr2, "kr2", t_kr2, bessel_correct);
	_print_stats(&ss_kr1, "kr1", t_kr1, bessel_correct);
	_print_stats(&ss_djb2, "djb2", t_djb2, bessel_correct);
	_print_stats(&ss_djb2xor, "djb2xor", t_djb2xor, bessel_correct);
	_print_stats(&ss_sdbm, "sdbm", t_sdbm, bessel_correct);
	_print_stats(&ss_paul, "paul", t_paul, bessel_correct);
	_print_stats(&ss_level, "level", t_level, bessel_correct);
	_print_stats(&ss_sip, "sip", t_sip, bessel_correct);
	_print_stats(&ss_murmur, "murmur", t_murmur, bessel_correct);
	_print_stats(&ss_xorrot, "xorrot", t_xorrot, bessel_correct);

	return 0;
}
