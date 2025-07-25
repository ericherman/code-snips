/* SPDX-License-Identifier: GPL-3.0-or-later */
/* minion-stats.c : a commandline utility for generating D&D 5e stats */
/* Copyright (C) 2019 Eric Herman <eric@freesa.org> */

/*
 * building requires a modern-ish c compiler as it uses some c99 code
 * constructs but is otherwise straightforward:
 *	cc -o minion-stats ./minion-stats.c
 * If you wish you can dail-up the cflags, but you're unlikely to notice
 * a difference:
 *	gcc -Wall -Wextra -g -O2 -DNDEBUG -o minion-stats ./minion-stats.c
 *
 * usage:
 *	./minion-stats $NUM_MINIONS $NUM_D6 $SORT_STATS
 *
 * defaults for parameters: 10 3 0
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
#include <sys/random.h>
#else
#include <time.h>
#endif

#define Default_num_stats 6
struct charstats {
	uint16_t *stats;
	size_t stats_len;
	int total_bonuses;
};

static int bonus_for_stat(int stat_val)
{
	return (stat_val / 2) - 5;
}

static int u16cmpdesc(const void *vpa, const void *vpb)
{
	const uint16_t a = (*(const uint16_t *)vpa);
	const uint16_t b = (*(const uint16_t *)vpb);
	return (b > a) ? 1 : (a > b) ? -1 : 0;
}

static int zcmpdesc(const void *vpa, const void *vpb)
{
	const size_t a = (*(const size_t *)vpa);
	const size_t b = (*(const size_t *)vpb);
	return (b > a) ? 1 : (a > b) ? -1 : 0;
}

#ifdef __linux__
#define init_rand() ((void)0)
static size_t rand_z(void)
{
	size_t val = 0;
	ssize_t r = 0;
	do {
		size_t flags = 0;
		r = getrandom(&val, sizeof(val), flags);
	} while (r < (ssize_t)sizeof(val));
	return val;
}
#else
#define init_rand() srand(time(NULL))
#define rand_z(void) ((size_t)rand())
#endif

void roll_stats(struct charstats *minion, size_t *dice, size_t num_dice,
		uint16_t die_max_value, size_t dice_to_keep, int sort_stats)
{
	assert(minion != NULL);
	assert(dice != NULL);
	assert(num_dice > 0);

	minion->total_bonuses = 0;
	for (size_t s = 0; s < minion->stats_len; ++s) {
		int stat = 0;
		for (size_t d = 0; d < num_dice; ++d) {
			size_t r = rand_z();
			dice[d] = (r % die_max_value) + 1;
		}

		/* sort to get the best dice_to_keep dice */
		assert(sizeof(dice[0]) == sizeof(size_t));
		qsort(dice, num_dice, sizeof(size_t), zcmpdesc);
		for (size_t d = 0; d < dice_to_keep && d < num_dice; ++d) {
			stat += (d < num_dice ? dice[d] : 1);
		}

		minion->stats[s] = stat;
		minion->total_bonuses += bonus_for_stat(stat);
	}

	if (sort_stats) {
		assert(sizeof(minion->stats[0]) == sizeof(uint16_t));
		qsort(minion->stats, minion->stats_len, sizeof(uint16_t),
		      u16cmpdesc);
	}
}

int charstats_cmpdesc(const void *a, const void *b)
{
	const struct charstats *sa = (const struct charstats *)a;
	const struct charstats *sb = (const struct charstats *)b;

	uint16_t lastats[2 * Default_num_stats];
	uint16_t *lbstats = lastats + Default_num_stats;

	/* copy so we can compare sorted stats on the copy and compare */
	struct charstats ca = { lastats, sa->stats_len, sa->total_bonuses };
	struct charstats cb = { lbstats, sb->stats_len, sb->total_bonuses };

	assert(sa->stats_len == sb->stats_len);
	if (sa->stats_len > Default_num_stats) {
		size_t total_size = 2 * sa->stats_len;
		ca.stats = (uint16_t *)calloc(total_size, sizeof(uint16_t));
		cb.stats = ca.stats + sa->stats_len;
	}
	memcpy(ca.stats, sa->stats, sizeof(uint16_t) * sa->stats_len);
	memcpy(cb.stats, sb->stats, sizeof(uint16_t) * sb->stats_len);

	if (cb.total_bonuses != ca.total_bonuses) {
		return (cb.total_bonuses - ca.total_bonuses);
	}

	assert(sizeof(ca.stats[0]) == sizeof(uint16_t));
	qsort(ca.stats, ca.stats_len, sizeof(uint16_t), u16cmpdesc);

	assert(sizeof(cb.stats[0]) == sizeof(uint16_t));
	qsort(cb.stats, cb.stats_len, sizeof(uint16_t), u16cmpdesc);

	assert(ca.stats_len == cb.stats_len);
	int rv = 0;
	for (size_t i = 0; i < ca.stats_len && !rv; ++i) {
		rv = cb.stats[i] - ca.stats[i];
	}

	if (ca.stats != lastats) {
		cb.stats = NULL;
		free(ca.stats);
		ca.stats = NULL;
	}

	return rv;
}

#include <stdio.h>
char *charstats_to_string(char *buf, size_t buf_len, struct charstats *minion)
{
	int printed = 0;
	size_t total_printed = 0;
	size_t s = 0;
	const char *plus = NULL;

	assert(buf);
	assert(buf_len);
	assert(minion);

	buf[0] = '\0';
	for (s = 0; s < minion->stats_len; ++s) {
		int stat = minion->stats[s];
		int bonus = bonus_for_stat(stat);
		plus = bonus >= 0 ? "+" : "";
		printed = snprintf(buf + total_printed, buf_len - total_printed,
				   "%2d (%s%d)  ", stat, plus, bonus);

		/* snprintf returns negative on error; if total_printed equals
		 * (or exceeds) buf_len, then data trucation occured */
		assert(printed > 0);
		total_printed += printed;
		assert(total_printed < buf_len);
	}
	plus = minion->total_bonuses >= 0 ? "+" : "";
	printed = snprintf(buf + total_printed, buf_len - total_printed,
			   " ((%s%d))", plus, minion->total_bonuses);

	assert(printed > 0);
	total_printed += printed;
	assert(total_printed < buf_len);

	return buf;
}


#ifndef NUM_STATS
#define NUM_STATS Default_num_stats
#endif

#ifndef DICE_TO_USE
#define DICE_TO_USE 3
#endif

#ifndef DIE_MAX_VALUE
#define DIE_MAX_VALUE 6
#endif

#if (DIE_MAX_VALUE > UINT16_MAX)
#error "(DIE_MAX_VALUE > UINT16_MAX)"
#endif

#ifndef Make_valgrind_happy
#ifdef NDEBUG
#define Make_valgrind_happy 0
#else
#define Make_valgrind_happy 1
#endif
#endif

int main(int argc, char **argv)
{
	/* the first argument is number of minions, default to 10 */
	size_t num_minions = 10;
	if (argc > 1) {
		int a = atoi(argv[1]);
		if (a > 0) {
			num_minions = a;
		}
	}

	/* second argument is number of dice to roll per stat, default to 3 */
	size_t num_dice = DICE_TO_USE;
	if (argc > 2) {
		int a = atoi(argv[2]);
		if (a > 0) {
			num_dice = a;
		}
	}

	/* the third is whether or not to sort the stats high to low */
	int sort_stats = 0;
	if (argc > 3) {
		sort_stats = atoi(argv[3]) ? 1 : 0;
	}

	/* make space for minions */
	struct charstats *minion_stats = NULL;
	minion_stats =
	    (struct charstats *)calloc(num_minions, sizeof(struct charstats));
	if (!minion_stats) {
		return EXIT_FAILURE;
	}
	size_t stats_size = sizeof(uint16_t) * NUM_STATS;
	uint8_t *stats = (uint8_t *)calloc(num_minions, stats_size);
	if (!stats) {
		return EXIT_FAILURE;
	}
	for (size_t i = 0; i < num_minions; ++i) {
		minion_stats[i].stats = (uint16_t *)(stats + (i * stats_size));
		minion_stats[i].stats_len = NUM_STATS;
	}

	/* make space for the dice */
	size_t ldice[DICE_TO_USE + 1];
	size_t *dice = ldice;
	if (num_dice > sizeof(ldice) / sizeof(ldice[0])) {
		dice = (size_t *)calloc(num_dice, sizeof(size_t));
		if (!dice) {
			return EXIT_FAILURE;
		}
	} else {
		memset(ldice, 0x00, sizeof(ldice));
	}

	/* roll stats for all the minions */
	init_rand();
	for (size_t i = 0; i < num_minions; ++i) {
		uint16_t die_max_value = DIE_MAX_VALUE;
		size_t dice_to_use = DICE_TO_USE;
		roll_stats(&minion_stats[i], dice, num_dice, die_max_value,
			   dice_to_use, sort_stats);
	}

	/* sort the minions from best to worst */
	qsort(minion_stats, num_minions, sizeof(struct charstats),
	      charstats_cmpdesc);

	/* print each minion */
	char buf[80];
	for (size_t i = 0; i < num_minions; ++i) {
		printf("%s\n", charstats_to_string(buf, 80, &minion_stats[i]));
	}

	if (Make_valgrind_happy) {
		if (dice != ldice) {
			free(dice);
			dice = NULL;
		}
		free(stats);
		stats = NULL;
		free(minion_stats);
		minion_stats = NULL;
	}

	return 0;
}
